#pragma once

#include <type_traits>
#include <METANN/policies/policy_container.hpp>

namespace METANN
{

///===Plain Policy===
template <typename TPolicyContainer, typename TResContainer>
struct PlainPolicy_
{
    using type = TResContainer;
};

template <typename TCurPolicy, typename...TPolicies, typename...TFilteredPolicies>
struct PlainPolicy_<PolicyContainer<TCurPolicy, TPolicies...>, PolicyContainer<TFilteredPolicies...>>
{
    using TNewFiltered = std::conditional_t<IsSubPolicyContainer<TCurPolicy>,
                                            PlainPolicy_<PolicyContainer<TPolicies...>, PolicyContainer<TFilteredPolicies...>>,
                                            PlainPolicy_<PolicyContainer<TPolicies...>, PolicyContainer<TFilteredPolicies..., TCurPolicy>>
                                            >;
    using type = typename TNewFiltered::type;
};

template <typename TPolicyContainer>
struct PlainPolicy : public PlainPolicy_<TPolicyContainer, PolicyContainer<>>::type{};

}