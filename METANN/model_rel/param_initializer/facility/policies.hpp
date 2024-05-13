#pragma once

#include <random>
#include <METANN/Facility/Policy.hpp>

namespace METANN
{

struct InitPolicy
{
    using MajorClass = InitPolicy;

    struct OverallTypeCate;
    struct WeightTypeCate;
    struct BiasTypeCate;

    //具体初始化器类型，默认为空类型
    using Overall = void;
    using Weight = void;
    using Bias = void;

    struct RandEngineTypeCate;
    using RandEngine = std::mt19937;
};

//具体Policy类模板声明
TypePolicyTemplate(PInitializerIs, InitPolicy, Overall);
TypePolicyTemplate(PWeightInitializerIs, InitPolicy, Weight);
TypePolicyTemplate(PBiasInitializerIs, InitPolicy, Bias);
TypePolicyTemplate(PRandomGeneratorIs, InitPolicy, RandEngine);


}