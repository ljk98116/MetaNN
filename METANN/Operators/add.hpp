#ifndef OP_ADD_HPP_
#define OP_ADD_HPP_

#include <type_traits>
#include <METANN/Operators/operators.hpp>
#include <METANN/Base/TrivialMatrix.hpp>
#include <METANN/Base/Duplicate.hpp>

namespace METANN
{

template <typename TP1, typename TP2>
class OperAdd_
{
private:
    using rawM1 = RemConstRef<TP1>;
    using rawM2 = RemConstRef<TP2>;
public:
    static constexpr bool valid = (IsMatrix<rawM1> && IsMatrix<rawM2>) ||
    (IsScalar<rawM1> && IsMatrix<rawM2>) || 
    (IsMatrix<rawM1> && IsScalar<rawM2>) ||
    (IsBatchMatrix<rawM1> && IsMatrix<rawM2>) ||
    (IsMatrix<rawM1> && IsBatchMatrix<rawM2>) ||
    (IsBatchMatrix<rawM1> && IsBatchMatrix<rawM2>) ||
    (IsBatchMatrix<rawM1> && IsScalar<rawM2>) ||
    (IsScalar<rawM1> && IsBatchMatrix<rawM2>);

    template <typename T1, typename T2>
    static constexpr bool Imp = std::is_same<T1, T2>::value;

    using CT = CategoryTags;

    template <typename T1, typename T2, std::enable_if_t<Imp<T1, T2>>* = nullptr>
    static auto Eval(TP1&& p_m1, TP2&& p_m2)
    {
        //检查元素类型和设备类型
        static_assert(
            std::is_same<typename rawM1::ElementType, typename rawM2::ElementType>::value, 
            "Matrices with different element types can not add directly"
        );
        static_assert(
            std::is_same<typename rawM1::DeviceType, typename rawM2::DeviceType>::value, 
            "Matrices with different device types can not add directly"
        );        
        using ResType = BinaryOp<BinaryOpTags::Add, rawM1, rawM2>;
        return ResType(std::forward<TP1>(p_m1), std::forward<TP2>(p_m2));
    }

    template <
        typename T1, typename T2, 
        std::enable_if_t<std::is_same<T1, CategoryTags::Matrix> ::value >* = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Scalar> ::value >* = nullptr
    >
    static auto Eval(TP1&& p_m1, TP2&& p_m2)
    {
        //检查元素类型和设备类型
        static_assert(
            std::is_same<typename rawM1::ElementType, typename rawM2::ElementType>::value, 
            "Matrices with different element types can not add directly"
        );
        static_assert(
            std::is_same<typename rawM1::DeviceType, typename rawM2::DeviceType>::value, 
            "Matrices with different device types can not add directly"
        );
        //标量转化为trivial_matrix
        auto tmpMatrix = MakeTrivialMatrix(p_m1.RowNum(), p_m1.ColNum(), p_m2);
        //转变为矩阵加法
        using ResType = BinaryOp<BinaryOpTags::Add, RemConstRef<decltype(tmpMatrix)>, rawM2>;
        return ResType(std::move(tmpMatrix), std::forward<TP2>(p_m2));        
    }

    template <
        typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::Scalar> ::value >* = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Matrix> ::value >* = nullptr        
    >
    static auto Eval(TP1&& p_m1, TP2&& p_m2)
    {
        return OperAdd_<TP2, TP1>:: template Eval<T2, T1>(std::forward<TP2>(p_m2), std::forward<TP1>(p_m1));
    }

    template <
        typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::BatchMatrix> ::value >* = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Matrix> ::value >* = nullptr
    >
    static auto Eval(TP1&& p_m1, TP2&& p_m2)
    {
        //检查元素类型和设备类型
        static_assert(
            std::is_same<typename rawM1::ElementType, typename rawM2::ElementType>::value, 
            "Matrices with different element types can not add directly"
        );
        static_assert(
            std::is_same<typename rawM1::DeviceType, typename rawM2::DeviceType>::value, 
            "Matrices with different device types can not add directly"
        );

        //复制矩阵
        Duplicate<rawM2> dupMatrix(std::forward<TP2>(p_m2), p_m1.BatchNum());

        using ResType = BinaryOp<BinaryOpTags::Add, rawM1, Duplicate<rawM2>>;
        return ResType(std::forward<TP1>(p_m1), std::move(dupMatrix));
    }

    template <
        typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::Matrix> ::value >* = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::BatchMatrix> ::value >* = nullptr
    >   
    static auto Eval(TP1&& p_m1, TP2&& p_m2)
    {
        return OperAdd_<TP2, TP1>::template Eval<T2, T1>(std::forward<TP2>(p_m2), std::forward<TP1>(p_m1));
    }

    template <
        typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::BatchMatrix> ::value >* = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::Scalar> ::value >* = nullptr        
    >
    static auto Eval(TP1&& p_m1, TP2&& p_m2)
    {
        //检查元素类型和设备类型
        static_assert(
            std::is_same<typename rawM1::ElementType, typename rawM2::ElementType>::value, 
            "Matrices with different element types can not add directly"
        );
        static_assert(
            std::is_same<typename rawM1::DeviceType, typename rawM2::DeviceType>::value, 
            "Matrices with different device types can not add directly"
        );

        //构造TrivialMatrix
        auto tmpMatrix = MakeTrivialMatrix(p_m1.RowNum(), p_m1.ColNum(), std::forward<TP2>(p_m2));

        auto dupMatrix = MakeDuplicate(p_m1.BatchNum(), std::move(tmpMatrix));
        using ResType = BinaryOp<BinaryOpTags::Add, rawM1, decltype(dupMatrix)>;
        return ResType(std::forward<TP1>(p_m1), std::move(dupMatrix));                
    }

    template <
        typename T1, typename T2,
        std::enable_if_t<std::is_same<T1, CategoryTags::Scalar> ::value >* = nullptr,
        std::enable_if_t<std::is_same<T2, CategoryTags::BatchMatrix> ::value >* = nullptr        
    >
    static auto Eval(TP1&& p_m1, TP2&& p_m2)
    {
        return OperAdd_<TP2, TP1>::template Eval<T2, T1>(std::forward<TP2>(p_m2), std::forward<TP1>(p_m1));
    }    
};

//加法有效时进行，获取最终操作对应的模板实例对象
template <
    typename TP1, typename TP2,
    std::enable_if_t<OperAdd_<TP1, TP2>::valid>* = nullptr
>
auto operator+(TP1&& p_m1, TP2&& p_m2)
{
    using T1 = DataCategory<TP1>;
    using T2 = DataCategory<TP2>;
    return OperAdd_<TP1, TP2>::template Eval<T1, T2>(std::forward<TP1>(p_m1), std::forward<TP2>(p_m2));
}

}

#endif