#ifndef OP_CATEGORY_CAL_HPP
#define OP_CATEGORY_CAL_HPP

#include <METANN/Operators/facility/traits.hpp>
#include <METANN/Base/DataCategory.hpp>
#include <tuple>

namespace METANN
{
template <typename TOpTag, typename THead, typename...TRemains>
struct OperCategory_;

namespace NSOperCateCal
{
    //将数据转换为对应类型
    template <typename TCateCont, typename ...TData>
    struct Data2Cate_
    {
        using type = TCateCont;
    };

    template <typename...TProcessed, typename TData, typename...TRemain>
    struct Data2Cate_<std::tuple<TProcessed...>, TData, TRemain...>
    {
        using tmp1 = std::tuple<TProcessed..., DataCategory<TData> >;
        using type = typename Data2Cate_<tmp1, TRemain...>::type;
    };

    template <typename THead, typename...TRemain>
    struct Data2Cate : public Data2Cate_<std::tuple<>, THead, TRemain...>::type{};

    template <typename TOpTag, typename TCateContainer>
    struct CateInduce_;

    template <typename TOptag, typename...TCates>
    struct CateInduce_<TOptag, std::tuple<TCates...> >
    {
        using type = typename OperCategory_<TOptag, TCates...>::type;
    };

    template <typename TCate, typename...TRemain>
    struct SameCate_
    {
        static constexpr bool value = true;
    };

    template <typename TCur1, typename TCur2, typename ...TRemain>
    struct SameCate_<TCur1, TCur2, TRemain...>
    {
        static constexpr bool value = ! std::is_same<TCur1, TCur2>::value ? false : SameCate_<TCur2, TRemain...>::value;
    };
}

template <typename TOptag, typename THead, typename...TRemain>
struct OperCategory_
{
    static_assert(NSOperCateCal::SameCate_<THead, TRemain...>::value, 
        "Date Category dismatch");
    using type = THead;
};

template <typename TOpTag, typename THead, typename...TRemain>
using OperCateCal = typename 
NSOperCateCal::CateInduce_<
    TOpTag, 
    NSOperCateCal::Data2Cate<THead, TRemain...> 
>::type;

}
#endif