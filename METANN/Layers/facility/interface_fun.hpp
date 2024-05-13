#pragma once

#include <type_traits>
#include <ostream>

namespace METANN
{

//检查层是否定义了对应的接口
namespace NSLayerInterface
{
template <typename TLayer, typename TInitializer, typename TBuffer, 
        typename TPolicies = typename TInitializer::PolicyCont
>
std::true_type InitTest(decltype(&TLayer::template Init<TInitializer, TBuffer, TPolicies>));

template <typename TLayer, typename TInitializer, typename TBuffer, 
        typename TPolicies = typename TInitializer::PolicyCont
>
std::false_type InitTest(...);

template <typename TLayer, typename TGradCollector>
std::true_type GradCollectTest(decltype(&TLayer:: template GradCollect<TGradCollector>));

template <typename TLayer, typename TGradCollector>
std::false_type GradCollectTest(...);

template <typename TLayer, typename TSaver>
std::true_type SaveWeightsTest(decltype(&TLayer:: template SaveWeights<TSaver>));

template <typename TLayer, typename TSaver>
std::false_type SaveWeightsTest(...);

template <typename TLayer>
std::true_type NeutralInvariantTest(decltype(&TLayer::NeutralInvariant));

template <typename TLayer>
std::false_type NeutralInvariantTest(...);

}

//层参数初始化与加载
template <typename TLayer, typename TInitializer, typename TBuffer, 
        typename TPolicies = typename TInitializer::PolicyCont
>
void LayerInit(TLayer& layer, TInitializer& initializer,
    TBuffer& loadBuffer, std::ostream& log=nullptr
)
{
    if constexpr (decltype(NSLayerInterface::InitTest<TLayer, TInitializer, TBuffer, TPolicies>(nullptr))::value)
    {
        layer.template Init<TInitializer, TBuffer, TPolicies>(initializer, loadBuffer, log);
    }
}

//收集参数梯度
template <typename TLayer, typename TGradCollector>
void LayerGradCollect(TLayer& layer, TGradCollector& gc)
{
    if constexpr (decltype(NSLayerInterface::GradCollectTest<TLayer, TGradCollector>(nullptr))::value)
    {
        layer.GradCollect(gc);
    }
}

//保存参数矩阵
template <typename TLayer, typename TSaver>
void SaveLayerWeight(TLayer& layer, TSaver& saver)
{
    if constexpr (decltype(NSLayerInterface::SaveWeightsTest<TLayer, TSaver>(nullptr))::value)
    {
        layer.SaveWeights(saver);
    }
}

template <typename TLayer>
void LayerNeutralInvariant(TLayer& layer)
{
    if constexpr (decltype(NSLayerInterface::NeutralInvariantTest<TLayer>(nullptr))::value)
    {
        layer.NeutralInvariant();
    }
}

template <typename TLayer, typename TIn>
auto LayerFeedForward(TLayer& layer, TIn&& p_in)
{
    return layer.FeedForward(std::forward<TIn>(p_in));
}

template <typename TLayer, typename TGrad>
auto LayerFeedBackward(TLayer& layer, TGrad&& p_grad)
{
    return layer.FeedBackward(std::forward<TGrad>(p_grad));
}

}