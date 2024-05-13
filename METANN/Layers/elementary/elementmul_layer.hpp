#pragma once

#include <METANN/Facility/VarTypeDict.hpp>
#include <METANN/Layers/facility/common_io.hpp>
#include <METANN/Layers/facility/interface_fun.hpp>
#include <METANN/Layers/facility/policies.hpp>
#include <METANN/Facility/Policy.hpp>
#include <METANN/Base/Dynamic.hpp>

namespace METANN
{

using ElementMulLayerInput = VarTypeDict<struct ElementMulLayerIn1, struct ElementMulLayerIn2>;

template <typename TPolicies>
class ElementMulLayer
{
    static_assert(IsPolicyContainer<TPolicies>, "TPolicies is not a PolicyContainer");
    using CurLayerPolicy = PlainPolicy<TPolicies>;
public:
    static constexpr bool IsFeedBackOutput = PolicySelect<FeedBackPolicy, CurLayerPolicy>::IsFeedBackOutput;
    static constexpr bool IsUpdate = false;
    using InputType = ElementMulLayerInput;
    using OutputType = LayerIO;

public:
    template <typename TIn>
    auto FeedForward(const TIn& p_in)
    {
        const auto& v1 = p_in.template Get<ElementMulLayerIn1>();
        const auto& v2 = p_in.template Get<ElementMulLayerIn2>();

        //获取v1,v2去除常量和引用后的类型，使用std::decay_t
        using rawType1 = std::decay_t<decltype(v1)>;
        using rawType2 = std::decay_t<decltype(v2)>;

        static_assert(!std::is_same<rawType1, NullParameter>::value, "param1 is invalid");
        static_assert(!std::is_same<rawType2, NullParameter>::value, "param2 is invalid");

        if constexpr(IsFeedBackOutput)
        {
            m_data1.push(MakeDynamic(v1));
            m_data2.push(MakeDynamic(v2));
        }
        return OutputType::Create().template Set<LayerIO>(v1 * v2);
    }

    template <typename TGrad>
    auto FeedBackward(TGrad&& p_grad)
    {
        if constexpr(IsFeedBackOutput)
        {
            if(m_data1.empty() || m_data2.empty())
            {
                throw std::runtime_error("No internal buf saved");
            }
            const auto& v1 = m_data1.top();
            const auto& v2 = m_data2.top();
            m_data1.pop();
            m_data2.pop();

            auto grad_eval = p_grad.template Get<LayerIO>();
            return ElementMulLayerInput::Create().template Set<ElementMulLayerIn1>(grad_eval * v1)
                                                .template Set<ElementMulLayerIn2>(grad_eval * v2);
        }
        else
        {
            return ElementMulLayerInput::Create();
        }
    }
    //中性检测，需要输出梯度时，检查缓存是否均为空
    void NeutralInvariant()
    {
        if constexpr(IsFeedBackOutput)
        {
            if((!m_data1.empty()) || (!m_data2.empty()))
            {
                throw std::runtime_error("NeutralInvariant fail");
            }
        }
    }
private:
    static constexpr bool BatchMode = PolicySelect<InputPolicy, CurLayerPolicy>::BatchMode;
    using ElementType = typename PolicySelect<OperandPolicy, CurLayerPolicy>::Element;
    using DeviceType = typename PolicySelect<OperandPolicy, CurLayerPolicy>::Device;
    //batchMode决定是Matrix还是BatchMatrix    
    using DataType = LayerTraits::LayerInternalBuf<IsFeedBackOutput, 
                        BatchMode, ElementType, DeviceType,
                        CategoryTags::Matrix, CategoryTags::BatchMatrix>;
    DataType m_data1, m_data2;        
};

}