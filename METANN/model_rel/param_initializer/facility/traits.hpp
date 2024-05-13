#pragma once

#include <METANN/model_rel/param_initializer/facility/policies.hpp>
#include <METANN/Facility/VarTypeDict.hpp>
#include <METANN/Facility/Policy.hpp>
#include <tuple>

namespace METANN
{

namespace NSParamInit
{
template <typename TPolicyCont, typename TGroup>
struct Group2Initializer_;

template <typename TPolicyCont>
struct Group2Initializer_<TPolicyCont, InitPolicy::WeightTypeCate>
{
    using type = typename PolicySelect<InitPolicy, TPolicyCont>::Weight;
};

template <typename TPolicyCont>
struct Group2Initializer_<TPolicyCont, InitPolicy::BiasTypeCate>
{
    using type = typename PolicySelect<InitPolicy, TPolicyCont>::Bias;
};

template <typename TPolicyCont, typename SpecInit>
struct PickInitializerBySpec
{
    using type = SpecInit;
};

template <typename TPolicyCont>
struct PickInitializerBySpec<TPolicyCont, void>
{
    using type = typename PolicySelect<InitPolicy, TPolicyCont>::Overall;
};

template <typename TPolicyCont, typename TSpecInitializer>
struct PickInitializer_
{
    static_assert(IsPolicyContainer<TPolicyCont>);
    using CurInitPolicy = PlainPolicy<TPolicyCont>;

    static_assert(!std::is_same<TSpecInitializer, InitPolicy::OverallTypeCate>::value);

    using SpecInitializer = typename Group2Initializer_<CurInitPolicy, TSpecInitializer>::type;

    using type = typename PickInitializerBySpec<CurInitPolicy, SpecInitializer>::type;
};

}

template <typename TPolicyCont, typename TSpecInitializer>
using PickInitializer = typename NSParamInit::PickInitializer_<TPolicyCont, TSpecInitializer>::type;

template <typename TRes, typename...TPolicies>
struct FilterTagFromPolicies_
{
    using type = TRes;
};

//针对不同的initializer进行的特化
template <typename...TProcessed, typename TCur, typename...TRemain>
struct FilterTagFromPolicies_<std::tuple<TProcessed...>, PInitializerIs<TCur>, TRemain...>
{
    using type = typename FilterTagFromPolicies_<std::tuple<TProcessed..., TCur>, TRemain...>::type; 
};

template <typename...TProcessed, typename TCur, typename...TRemain>
struct FilterTagFromPolicies_<std::tuple<TProcessed...>, PWeightInitializerIs<TCur>, TRemain...>
{
    using type = typename FilterTagFromPolicies_<std::tuple<TProcessed..., TCur>, TRemain...>::type; 
};

template <typename...TProcessed, typename TCur, typename...TRemain>
struct FilterTagFromPolicies_<std::tuple<TProcessed...>, PBiasInitializerIs<TCur>, TRemain...>
{
    using type = typename FilterTagFromPolicies_<std::tuple<TProcessed..., TCur>, TRemain...>::type; 
};

template <typename TCheck, typename ...TRem>
struct AlreadyExist
{
    static constexpr bool value = false;
};

template <typename TCheck, typename TCur, typename...TRem>
struct AlreadyExist<TCheck, TCur, TRem...>
{
    static constexpr bool value = !std::is_same<TCheck, TCur>::value ? AlreadyExist<TCheck, TRem...>::value : true;
};

template <typename TRes, typename...TTags>
struct TagsDeque_
{
    using type = TRes;
};

template <typename ...TProcessed, typename TCur, typename...TRem>
struct TagsDeque_<VarTypeDict<TProcessed...>, TCur, TRem...>
{
    using type = std::conditional_t<
    AlreadyExist<TCur, TProcessed...>::value,
    typename TagsDeque_<VarTypeDict<TProcessed...>, TRem...>::type,
    typename TagsDeque_<VarTypeDict<TProcessed..., TCur>, TRem...>::type
    >;
};


//接收TPolicies列表，返回标签的tuple,利用特化加入tuple中的标签到VarTypeDict结构
template <typename...TPolicies>
struct FillerTags2NamedParams_
{
    using step1 = typename FilterTagFromPolicies_<std::tuple<>, TPolicies...>::type;
    using type = typename TagsDeque_<VarTypeDict<>, step1>::type;
};

template <typename...TPolicies>
struct FillerTags2NamedParams : public FillerTags2NamedParams_<TPolicies...>::type
{};

}