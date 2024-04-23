#ifndef POLICY_HPP_
#define POLICY_HPP_
#include <cmath>

namespace NSVarTypeDict
{

//policy组，累积操作的策略
struct AccPolicy
{
    //累积操作类型
    struct AccuTypeCate
    {
        struct Add;
        struct Mul;
    };
    //编译期键值对,默认采用加法类型进行累积
    using Accu = AccuTypeCate::Add;

    //是否取均值
    struct IsAveValueCate;
    static constexpr bool IsAve = false;

    //返回值类型
    struct ValueTypeCate;
    using Value = float;
};

//policy容器
template <typename ...TPolicies>
struct PolicyContainer;

//policy选择元函数
//首先选取MajorClass相同的Policy,然后检查MinorClass确保不会互斥
//返回最终的Policy选择结果
namespace NSPolicySelect
{
//检测当前布尔值，如果false返回false，否则取类型t2的value，即计算t2
//短路求值
template <bool t1, typename t2>
static constexpr bool AndValue = !t1 ? false : t1 && t2::value;

//判断数组是否为空
template <typename TPolicyCont>
struct IsArrayEmpty_
{
    static constexpr bool value = true;
};

template <typename ...TPolicies>
struct IsArrayEmpty_<PolicyContainer<TPolicies...>>
{
    static constexpr bool value = sizeof...(TPolicies) == 0;
};

//判断数组是否为空, 外层元函数
template <typename TPolicyCont>
static constexpr bool IsArrayEmpty = IsArrayEmpty_<TPolicyCont>::value;

template <typename TPolicyCont>
struct PolicySelRes;

template <typename TPolicy>
struct PolicySelRes<PolicyContainer<TPolicy>> : public TPolicy
{};

template <typename TPolicy, typename...OtherTPolicies>
struct PolicySelRes<PolicyContainer<TPolicy, OtherTPolicies...> > : 
public TPolicy, public PolicySelRes<PolicyContainer<OtherTPolicies...> >
{};

//检查Minor类型是否冲突
template <typename TMinorClass, typename TPolicyCont>
struct Minor_Dup_
{
    static constexpr bool value = true;
};

template <typename TMinorClass, typename CurPolicy, typename...TPolicies>
struct Minor_Dup_<TMinorClass, PolicyContainer<CurPolicy, TPolicies...>>
{
    using TMinorMirror = typename CurPolicy::MinorClass;
    static constexpr bool curcheck = !(std::is_same<TMinorClass, TMinorMirror>::value);
    static constexpr bool value = AndValue<curcheck, Minor_Dup_<TMinorClass, PolicyContainer<TPolicies...> > >;
};

//检查MinorClass中的冲突项元函数
template <typename TPolicyCont>
struct Minor_Check
{
    static constexpr bool value = true;
};

template <typename CurPolicy, typename...TPolicies>
struct Minor_Check<PolicyContainer<CurPolicy, TPolicies...>>
{
    //利用当前Policy的MinorClass去后续序列中找
    static constexpr bool curcheck = Minor_Dup_<typename CurPolicy::MinorClass, PolicyContainer<TPolicies...> >::value;
    static constexpr bool value = AndValue<curcheck, Minor_Check<PolicyContainer<TPolicies...> > >;
};

//选取MajorClass为TMajorClass的Policy
template <typename TPolicyCont, typename TMajorClass, typename...TPolicies>
struct MajorFilter_;

template <typename TMajorClass, 
        typename TCurPolicy, 
        typename...TPolicies,
        typename...ModifiedTPolicies
        >
struct MajorFilter_<
    PolicyContainer<ModifiedTPolicies...>, TMajorClass, 
    TCurPolicy, TPolicies...
>
{
    using MajorClassMirror = typename TCurPolicy::MajorClass;
    using type = std::conditional_t<
        std::is_same<MajorClassMirror, TMajorClass>::value,
        typename MajorFilter_<
            PolicyContainer<ModifiedTPolicies..., TCurPolicy>, 
            TMajorClass, 
            TPolicies...
            >::type,
        typename MajorFilter_<
            PolicyContainer<ModifiedTPolicies...>, 
            TMajorClass, 
            TPolicies...            
        >::type
    >;
    //static_assert(std::is_same<type, PolicyContainer<ModifiedTPolicies...>>::value);
};

template <typename TMajorClass, typename...ModifiedTPolicies>
struct MajorFilter_<PolicyContainer<ModifiedTPolicies...>, TMajorClass>
{
    using type = PolicyContainer<ModifiedTPolicies...>;
};

template <typename TMajorClass, typename TPolicyContainer>
struct Selector_;

//限定容器必须是PolicyContainer类型
template <typename TMajorClass, typename ...TPolicies>
struct Selector_<TMajorClass, PolicyContainer<TPolicies...>>
{
    using TMF = typename MajorFilter_<PolicyContainer<>, TMajorClass, TPolicies...>::type;
    static_assert(Minor_Check<TMF>::value, "Minor Class Set Conflict !!");
    using type = std::conditional_t<IsArrayEmpty<TMF>, TMajorClass, PolicySelRes<TMF>>;
};
}

template <typename TMajorClass, typename TPolicyContainer>
using PolicySelect = typename NSPolicySelect::Selector_<TMajorClass, TPolicyContainer>::type;

//具体Policy
//乘积累积
#if 0
struct PMulAccu : virtual public AccPolicy
{
    //所属policy组的类型
    using MajorClass = AccPolicy;
    //互斥信息，即通过AccPolicy::AccuTypeCate来进行区分
    using MinorClass = typename AccPolicy::AccuTypeCate;
    using Accu = typename AccPolicy::AccuTypeCate::Mul;
};
#endif

//policy类型构建器
#define TypePolicyObj(Policyname, Ma, Mi, Val) \
struct Policyname : virtual public Ma \
{ \
    using MajorClass = Ma;\
    using MinorClass = typename Ma::Mi##TypeCate; \
    using Mi = typename Ma::Mi##TypeCate::Val;\
};

#define ValuePolicyObj(Policyname, Ma, Mi, Val) \
struct Policyname : virtual public Ma \
{ \
    using MajorClass = Ma; \
    using MinorClass = typename Ma::Mi##ValueCate; \
    static constexpr decltype(Val) Mi = Val; \
}; \

#define TypePolicyTemplate(Policyname, Ma, Mi) \
template <typename T> \
struct Policyname : virtual public Ma\
{ \
    using MajorClass = Ma; \
    using MinorClass = typename Ma::Mi##TypeCate; \
    using Mi = T; \
};

#define ValuePolicyTemplate(Policyname, Ma, Mi) \
template <typename T> \
struct Policyname : virtual public Ma\
{ \
    using MajorClass = Ma; \
    using MinorClass = typename Ma::Mi##ValueCate; \
    using Mi = T; \
};

template <typename T>
static constexpr bool DependencyFalse = false;

//policy用例
template <typename ...TPolicies>
struct Accumulator
{
    //Policy容器
    using PoliCont = PolicyContainer<TPolicies...>;

    //Policy选取，从Policies中选取AccPolicy作为Policy选取的结果
    using TPolicyRes = PolicySelect<AccPolicy, PoliCont>;

    using ValueType = typename TPolicyRes::Value;
    static constexpr bool isAve = TPolicyRes::IsAve;
    using AccuType = typename TPolicyRes::Accu;
public:
    //执行累积操作
    template <typename TIn>
    static auto Eval(const TIn& in)
    {
        //编译期执行类型判断
        if constexpr(std::is_same<AccuType, typename AccPolicy::AccuTypeCate::Add >::value)
        {
            ValueType count = 0;
            ValueType res = 0;
            for(const auto& x: in)
            {
                res += x;
                count += 1;
            }
            if constexpr(isAve) res = res / count;
            return res;
        }
        else if constexpr(std::is_same<AccuType, typename AccPolicy::AccuTypeCate::Mul >::value)
        {
            ValueType count = 0;
            ValueType res = 1;
            for(const auto& x: in)
            {
                res *= x;
                count += 1;
            }
            if constexpr(isAve) res = pow(res, 1.0 / count);
            return res;            
        }
        else
        {
            static_assert(DependencyFalse<AccuType>);
        }
    }
};

//定义不同Policy
TypePolicyObj(PAddAccu, AccPolicy, Accu, Add);
TypePolicyObj(PMulAccu, AccPolicy, Accu, Mul);
ValuePolicyObj(PAve, AccPolicy, IsAve, true);
ValuePolicyObj(PNoAve, AccPolicy, IsAve, false);
TypePolicyTemplate(PValueTypeIs, AccPolicy, Value);
ValuePolicyTemplate(PAvePolicyIs, AccPolicy, IsAve);

}
#endif