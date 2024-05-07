#ifndef OP_SIGMOID_HPP_
#define OP_SIGMOID_HPP_

#include <type_traits>
#include <METANN/Operators/operators.hpp>

namespace METANN
{

template <typename TP>
class OperSigmoid_
{
private:
    using RawM = RemConstRef<TP>;
public:
    static constexpr bool valid = IsMatrix<RawM> || IsBatchMatrix<RawM>;

public:
    static auto Eval(TP&& p_m)
    {
        using ResType = UnaryOp<UnaryOpTags::Sigmoid, RawM>;
        return ResType(std::forward<TP>(p_m));
    }
};

//当OperSigmoid_<TP>::valid为true时，该函数可以编译
template <typename TP, std::enable_if_t<OperSigmoid_<TP>::valid >* =nullptr>
auto Sigmoid(TP&& p_m)
{
    return OperSigmoid_<TP>::Eval(std::forward<TP>(p_m));
}


}

#endif