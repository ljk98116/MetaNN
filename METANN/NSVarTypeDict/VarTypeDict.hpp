#ifndef VARTYPEDICT_HPP_
#define VARTYPEDICT_HPP_

#include <memory>
#include <iostream>
#include <vector>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/equal.hpp>

/// @brief 
/// 异类词典实现
/// 编译期键值，即类型量
/// 运行期值，通过new来保存，通过空类型shared_ptr vector保存，通过类型序列建立键值的对应关系
namespace NSVarTypeDict
{
//空类型
struct NullParameter{};

//递归元函数构造序列
//当待构造数量为0时将元素序列加入容器
template <size_t N, template <typename...> typename TCont, typename...T>
struct Create_
{
    using type = typename Create_<N-1, TCont, NullParameter, T...>::type;
};

template <template <typename...> typename TCont, typename...T>
struct Create_<0, TCont, T...>
{
    using type = TCont<T...>;
};

//Tag2ID,在类型序列中查找对应的Tag位置
//取类型序列的第一个元素
template <size_t N, 
        typename TTag,
        typename ...TParameters>
struct Tag2ID_imp
{
    constexpr static size_t value = N;
};

template <size_t N, typename TTag, 
        typename T, 
        typename...TParameters>
struct Tag2ID_imp<N, TTag, T, TParameters...>
{
    //判断类型是否相等
    constexpr static size_t value = std::is_same<T, TTag>::value ? 
    N : (sizeof...(TParameters) == 0 ? N+1 : Tag2ID_imp<N+1, TTag, TParameters...>::value);
};

//constexpr含编译期参数表达式
template <typename TTag, typename...TParameters>
constexpr size_t Tag2ID = Tag2ID_imp<0, TTag, TParameters...>::value;

//NewTupleType
//对类型序列对应位置元素赋值
template <typename TVal, size_t N, size_t M,
        typename TProcessedTypes,
        typename ...TRemainTypes>
struct NewTupleType_;

//还没有找到位置
template <typename TVal, size_t N, size_t M,
        template <typename...> typename TCont,
        typename ...TModifiedTypes,
        typename TCurType,
        typename ...TRemainTypes>
struct NewTupleType_<TVal, N, M, TCont<TModifiedTypes...>, TCurType, TRemainTypes...>
{
    using type = typename NewTupleType_<TVal, N, M+1, TCont<TModifiedTypes..., TCurType>,  TRemainTypes...>::type;
};

//已经找到了对应位置
template <typename TVal, size_t N,
        template <typename...> typename TCont,
        typename ...TModifiedTypes,
        typename TCurType,
        typename ...TRemainTypes>
struct NewTupleType_<TVal, N, N, TCont<TModifiedTypes...>, TCurType, TRemainTypes...>
{
    using type = TCont<TModifiedTypes..., TVal, TRemainTypes...>;
};

//外壳
template <typename TVal, size_t TagPos,typename TCont, typename...TRemainTypes>
struct NewTupleType
{
    using type = typename NewTupleType_<TVal, TagPos, 0, TCont, TRemainTypes...>::type;
};

//TupleLocate找到对应位置的类型
template<size_t TagPos, size_t N, typename TCurtype, typename...TCont>
struct TupleLocate_imp
{
    using type = typename TupleLocate_imp<TagPos, N+1, TCont...>::type;
};

template<size_t TagPos, typename TCurtype, typename...TCont>
struct TupleLocate_imp<TagPos, TagPos, TCurtype, TCont...>
{
    using type = TCurtype;
};

template<size_t TagPos, typename...TCont>
struct TupleLocate
{
    using type = typename TupleLocate_imp<TagPos, 0, TCont...>::type;
};

template <typename ...TParameters>
struct VarTypeDict
{
    class TagNotFoundException : std::exception
    {
    public:
        const char *what() const noexcept
        {
            //std::cout << "Not Found" << std::endl;
            return "Not Found\n";
        }
    };
    //Values声明
    template <typename...TTypes>
    struct Values
    {
    public:
        Values(std::vector<std::shared_ptr<void>> input):m_tuple(std::move(input)){}
        Values():m_tuple(std::vector<std::shared_ptr<void>>(sizeof...(TTypes))){}
    public:
        template <typename TTag, typename TValue>
        auto Set(TValue &&val) &&
        {
            using namespace NSVarTypeDict;
            //找到TTag的位置
            constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;
            //在堆中创建对应类型的值的副本
            using rawVal = std::decay_t<TValue>;
            rawVal *tmp = new rawVal(std::forward<TValue>(val));
            //给出对应类型的Deleter，用来释放内存
            m_tuple[TagPos] = std::shared_ptr<void>(
                tmp,
                [](void *ptr)
                {
                    rawVal *nptr = static_cast<rawVal*>(ptr);
                    delete nptr;
                }
            );
            //利用当前的Set操作修改当前的Values类型
            //返回了一个新的Value类型，复用当前的m_tuple，避免内存申请释放
            using new_type = typename NewTupleType<rawVal, TagPos, Values<>, TTypes...>::type;
            //static_assert(boost::mpl::equal<new_type, Values<bool,NullParameter,NullParameter>>::value);
            return new_type(std::move(m_tuple));
        }
        //返回类型对应的值
        template <typename TTag>
        auto& Get() const
        {
            //存在问题
            //找到对应的Tag的位置
            constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;
            if(TagPos == -1) 
            {
                //std::cout << typeid(TTag).name();
                throw new TagNotFoundException();
            }
            //找到TTag对应的类型
            using TValue = std::decay_t<typename TupleLocate<TagPos, TTypes...>::type>;
            return *static_cast<TValue*>(m_tuple[TagPos].get());
        }
    private:
        //std::shared_ptr<void> m_tuple[sizeof...(TTypes)];
        std::vector<std::shared_ptr<void>> m_tuple;
    };

public:
    static auto Create()
    {
        using namespace NSVarTypeDict;
        //类型序列初始为空
        using type = typename Create_<sizeof...(TParameters), Values>::type;
        return type{};
    }
};

template <typename TDict, typename Item, typename...CheckedItems>
struct addItem_imp;

template <typename...RestItems, typename CurItem, typename Item, typename...CheckedItems>
struct addItem_imp<VarTypeDict<CurItem, RestItems...>, Item, CheckedItems...>
{
    //查询到相同键值
    static_assert(!std::is_same<CurItem, Item>::value, "conflict key in vartypedict !!");
    using type = typename addItem_imp<VarTypeDict<RestItems...>, Item, CheckedItems..., CurItem>::type;
};

template <typename Item, typename...CheckedItems>
struct addItem_imp<VarTypeDict<>, Item, CheckedItems...>
{
    using type = VarTypeDict<CheckedItems..., Item>;
};

//addItem元函数, 本身为VarTypeDict
//addItem需要检查Dict中每个元素后，将Key加入
template <typename Dict, typename Item>
struct addItem : addItem_imp<Dict, Item>::type
{};

//delItem元函数
//delItem需要检查Dict每个元素看是否存在一致，将对应Item删除
template <typename TDict, typename Item, typename...CheckedItems>
struct delItem_imp;

template <typename...RestItems, typename CurItem, typename Item, typename...CheckedItems>
struct delItem_imp<VarTypeDict<CurItem, RestItems...>, Item, CheckedItems...>
{
    using type = std::conditional_t<
        std::is_same<CurItem, Item>::value,
        VarTypeDict<CheckedItems..., RestItems...>,
        typename delItem_imp<VarTypeDict<RestItems...>, Item, CheckedItems..., CurItem>::type
    >;
};

template <typename LastItem, typename Item, typename...CheckedItems>
struct delItem_imp<VarTypeDict<LastItem>, Item, CheckedItems...>
{
    static_assert(!std::is_same<LastItem, Item>::value, "Dict Item not exist !!");
    using type = VarTypeDict<CheckedItems...>;
};

template <typename TDict, typename Item>
struct delItem : delItem_imp<TDict, Item>::type{};

}
#endif