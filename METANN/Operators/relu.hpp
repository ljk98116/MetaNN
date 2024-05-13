#ifndef OP_RELU_HPP_
#define OP_RELU_HPP_

#include <type_traits>
#include <METANN/Operators/operators.hpp>

namespace METANN
{

template <typename TP>
class OperRelu_
{
    using rawM = RemConstRef<TP>;
public:
    static constexpr bool valid = IsMatrix<rawM> || IsBatchMatrix<rawM>;
public:
public:
    static auto Eval(TP&& p_m)
    {
        using ResType = UnaryOp<UnaryOpTags::Relu, rawM>;
        return ResType(std::forward<TP>(p_m));
    }    
};

template <
    typename TP,
    std::enable_if_t<OperRelu_<TP>::valid>* = nullptr
>
auto Relu(TP&& p_m)
{
    return OperRelu_<TP>::Eval(std::forward<TP>(p_m));
}

}
#endif