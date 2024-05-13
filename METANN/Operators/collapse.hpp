#ifndef OP_COLLAPSE_HPP_
#define OP_COLLAPSE_HPP_

#include <type_traits>
#include <METANN/Operators/operators.hpp>
#include <METANN/Operators/facility/category_cal.hpp>

namespace METANN
{

//接收矩阵列表返回矩阵
template <typename TP>
class OperCollapse_
{
private:
    using rawM = RemConstRef<TP>;
public:
    static constexpr bool valid = IsBatchMatrix<rawM>;
public:
    static auto Eval(TP&& p_m)
    {
        using ResType = UnaryOp<UnaryOpTags::Collapse, rawM>;
        return ResType(std::forward<TP>(p_m));
    }
};

//特化OperCategory
template<>
struct OperCategory_<UnaryOpTags::Collapse, CategoryTags::BatchMatrix>
{
    using type = CategoryTags::Matrix;
};

template <
    typename TP,
    std::enable_if_t<OperCollapse_<TP>::valid>* = nullptr
>
auto Collapse(TP&& p_m)
{
    return OperCollapse_<TP>::Eval(std::forward<TP>(p_m));    
}

}

#endif