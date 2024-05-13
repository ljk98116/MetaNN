//避免宏名称撞车，一般不支持跨平台
#pragma once

//#ifndef移植性好，但大型项目会出现宏名称撞车问题
#include <METANN/policies/policy_container.hpp>
namespace METANN
{

template <template <typename TPolicyCont> typename T, typename...TPolicies>
using InjectPolicy = T<PolicyContainer<TPolicies...> >;
}