#pragma once

#include <METANN/Facility/VarTypeDict.hpp>
#include <METANN/Layers/facility/common_io.hpp>
#include <METANN/Layers/facility/interface_fun.hpp>
#include <METANN/Layers/facility/policies.hpp>
#include <METANN/Facility/Policy.hpp>
#include <METANN/Base/Dynamic.hpp>
#include <METANN/policies/policy_container.hpp>
#include <METANN/Layers/facility/traits.hpp>
#include <METANN/model_rel/param_initializer/facility/traits.hpp>

namespace METANN
{
//内部保存一个偏置矩阵，外部输入一个矩阵，输出一个矩阵
template <typename TPolicies>
class BiasLayer
{
    static_assert(IsPolicyContainer<TPolicies>, "TPolicies is not a PolicyContainer");
    using CurLayerPolicy = PlainPolicy<TPolicies>;
public:
    static constexpr bool IsFeedBackOutput = PolicySelect<FeedBackPolicy, CurLayerPolicy>::IsFeedBackOutput;
    static constexpr bool IsUpdate = PolicySelect<FeedBackPolicy, CurLayerPolicy>::IsUpdate;
    using InputType = LayerIO;
    using OutputType = LayerIO;
private:
    using ElementType = typename PolicySelect<OperandPolicy, CurLayerPolicy>::Element;
    using DeviceType = typename PolicySelect<OperandPolicy, CurLayerPolicy>::Device;
public:
    BiasLayer(std::string p_name, size_t p_vecLen):
    m_name(std::move(p_name))
    {
        if(p_vecLen == 0)
        {
            throw std::runtime_error("Invalid input length for bias layer");
        }
        m_rowNum = 1;
        m_colNum = p_vecLen;
    }
    BiasLayer(std::string p_name, size_t p_rowNum, size_t p_colNum):
    m_name(std::move(p_name)), m_rowNum(p_rowNum), m_colNum(p_colNum)
    {
        if(m_rowNum == 0 || m_colNum == 0)
        {
            throw std::runtime_error("Invalid input row/col num for bias layer");
        }
    }

public:
    template <typename TInitializer, typename TBuffer, typename TInitPolicies=typename TInitializer::TPolicyCont>
    void Init(TInitializer& initializer, TBuffer& loadBuffer, std::ostream* log=nullptr)
    {
        //先找loadBuffer，再找initializer，最后重新新建
        //找对应名称的矩阵
        auto cit = loadBuffer.find(m_name);
        if(cit != loadBuffer.end())
        {
            const Matrix<ElementType, DeviceType>& m = cit->second;
            if((m.RowNum() != m_rowNum) || (m.ColNum() != m_colNum))
            {
                throw std::runtime_error("Load matrix error in bias layer");
            }
            m_bias = m;
            if(log)
            {
                std::string logInfo = "Load from loadBuffer" + m_name + '\n';
                (*log) << logInfo;
            }
            return;
        }
        else if(initializer.IsMatrixExist(m_name))
        {
            m_bias = Matrix<ElementType, DeviceType>(m_rowNum, m_colNum);
            initializer.GetMatrix(m_name, m_bias);
            loadBuffer[m_name] = m_bias;
            if(log)
            {
                std::string logInfo = "Copy from initializer" + m_name + '\n';
                (*log) << logInfo;                
            }
            return;
        }
        else
        {
            m_bias = Matrix<ElementType, DeviceType>(m_rowNum, m_colNum);
            using CurInitializer = PickInitializer<TInitPolicies, InitPolicy::BiasTypeCate>;
            //CurInitializer被指定了，按对应的initializer填充bias矩阵
            if constexpr (!std::is_same<CurInitializer, void>::value)
            {
                size_t fan_io = m_rowNum * m_colNum;
                auto& cur_init = initializer.template GetFiller<CurInitializer>();
                cur_init.Fill(m_bias, fan_io, fan_io);
                loadBuffer[m_name] = m_bias;
                if(log)
                {
                    std::string logInfo = "Random init from initializer" + m_name + '\n';
                    (*log) << logInfo;                      
                }
            }
            else
            {
                throw std::runtime_error("Cannot get initializer for InitPolicy::BiasTypeCate");
            }
        }
    }

    template <typename TSaver>
    void SaveWeights(TSaver& saver) const
    {
        auto cit = saver.find(m_name);
        //找到了名称相同但值不为m_bias的矩阵，报错
        if((cit != saver.end()) && (cit->second != m_bias))
        {
            throw std::runtime_error(m_name + "bias is already exist.");
        }
        saver[m_name] = m_bias;
    }

    template <typename TIn>
    auto FeedForward(const TIn& p_in)
    {
        const auto& val = p_in.template Get<LayerIO>();
        return LayerIO::Create().template Set<LayerIO>(val + m_bias);
    }

    template <typename TGrad>
    auto FeedBackward(const TGrad& p_grad)
    {
        if constexpr (IsUpdate)
        {
            const auto& tmp = p_grad.template Get<LayerIO>();
            assert((tmp.RowNum() == m_rowNum) && (tmp.ColNum() == m_colNum));
            m_grad.push(MakeDynamic(tmp));
        }
        if constexpr (IsFeedBackOutput)
        {
            return p_grad;
        }
        else
        {
            return LayerIO::Create();
        }
    }

    template <typename TGradCollector>
    void GradCollect(TGradCollector& col)
    {
        if constexpr (IsUpdate)
        {
            LayerTraits::MatrixGradCollect(m_bias, m_grad, col);
        }
    }

    void NeotralInvariant() const
    {
        if constexpr(IsUpdate)
        {
            if(!m_grad.empty())
            {
                throw std::runtime_error("NeotralInvariant fail");
            }
        }
    }
private:
    const std::string m_name;
    size_t m_rowNum, m_colNum;

    Matrix<ElementType, DeviceType> m_bias;

    using DataType = LayerTraits::LayerInternalBuf<IsUpdate,
                                    PolicySelect<InputPolicy, CurLayerPolicy>::BatchMode,
                                    ElementType, DeviceType,
                                    CategoryTags::Matrix, CategoryTags::BatchMatrix
                                    >;
    DataType m_grad;
};

}