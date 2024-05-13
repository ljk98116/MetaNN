#pragma once

#include <METANN/Facility/VarTypeDict.hpp>
#include <METANN/Layers/facility/common_io.hpp>
#include <METANN/Layers/facility/interface_fun.hpp>
#include <METANN/Layers/facility/policies.hpp>
#include <METANN/Facility/Policy.hpp>
#include <METANN/policies/policy_operations.hpp>

namespace METANN
{

using AddLayerInput = VarTypeDict<struct AddLayerIn1, struct AddLayerIn2>;

template <typename TPolicies>
class AddLayer
{
    static_assert(IsPolicyContainer<TPolicies>, "TPolicies is not a PolicyContainer");
    using CurLayerPolicy = PlainPolicy<TPolicies>;

public:
    static constexpr bool IsFeedBackOutput = PolicySelect<FeedBackPolicy, CurLayerPolicy>::IsFeedBackOutput;
    static constexpr bool IsUpdate = false;
    using InputType = AddLayerInput;
    using OutputType = LayerIO;

public:
    template <typename TIn>
    auto FeedForward(const TIn& p_in)
    {
        const auto& v1 = p_in.template Get<AddLayerIn1>();
        const auto& v2 = p_in.template Get<AddLayerIn2>();

        //获取v1,v2去除常量和引用后的类型，使用std::decay_t
        using rawType1 = std::decay_t<decltype(v1)>;
        using rawType2 = std::decay_t<decltype(v2)>;

        static_assert(!std::is_same<rawType1, NullParameter>::value, "param1 is invalid");
        static_assert(!std::is_same<rawType2, NullParameter>::value, "param2 is invalid");

        return OutputType::Create().template Set<LayerIO>(v1 + v2);
    }

    template <typename TGrad>
    auto FeedBackward(TGrad&& p_grad)
    {
        if constexpr(IsFeedBackOutput)
        {
            auto res = p_grad. template Get<LayerIO>();
            return AddLayerInput::Create().template Set<AddLayerIn1>(res). template Set<AddLayerIn2>(res);
        }
        else
        {
            return AddLayerInput::Create();
        }
    }
};

}