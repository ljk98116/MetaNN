#ifndef DATACATEGORY_HPP_
#define DATACATEGORY_HPP_

namespace METANN
{
//去常量、引用属性
template <typename T>
struct RemConstRefImpl
{
    using type = T;
};

template <typename T>
struct RemConstRefImpl<const T>
{
    using type = typename RemConstRefImpl<T>::type;
};

template <typename T>
struct RemConstRefImpl<T&>
{
    using type = typename RemConstRefImpl<T>::type;
};

template <typename T>
struct RemConstRef : typename RemConstRefImpl<T>::type{};

//数据类型的类声明
template <typename TElem, typename TDevice>
struct Scalar;

template <typename TElem, typename TDevice>
struct Matrix;


//类型标签：标量、矩阵、标量列表、矩阵列表
struct CategoryTags
{
    struct Scalar;
    struct Matrix;
    struct BatchScalar;
    struct BatchMatrix;
};

//类型、标签关联元函数

//判断类型是否属于某个标签
template <typename T>
constexpr bool IsScalar = false;

template <typename T>
constexpr bool IsScalar<const T> = IsScalar<T>;

template <typename T>
constexpr bool IsScalar<T&> = IsScalar<T>;

template <typename T>
constexpr bool IsScalar<T&&> = IsScalar<T>;

template <typename T>
constexpr bool IsScalar<const T&&> = IsScalar<T>;

template <typename T>
constexpr bool IsMatrix = false;

template <typename T>
constexpr bool IsMatrix<const T> = IsMatrix<T>;

template <typename T>
constexpr bool IsMatrix<T&> = IsMatrix<T>;

template <typename T>
constexpr bool IsMatrix<T&&> = IsMatrix<T>;

template <typename T>
constexpr bool IsMatrix<const T&&> = IsMatrix<T>;

template <typename T>
constexpr bool IsBatchScalar = false;

template <typename T>
constexpr bool IsBatchScalar<const T> = IsBatchScalar<T>;

template <typename T>
constexpr bool IsBatchScalar<T&> = IsBatchScalar<T>;

template <typename T>
constexpr bool IsBatchScalar<T&&> = IsBatchScalar<T>;

template <typename T>
constexpr bool IsBatchScalar<const T&&> = IsBatchScalar<T>;

template <typename T>
constexpr bool IsBatchMatrix = false;

template <typename T>
constexpr bool IsBatchMatrix<const T> = IsBatchMatrix<T>;

template <typename T>
constexpr bool IsBatchMatrix<T&> = IsBatchMatrix<T>;

template <typename T>
constexpr bool IsBatchMatrix<T&&> = IsBatchMatrix<T>;

template <typename T>
constexpr bool IsBatchMatrix<const T&&> = IsBatchMatrix<T>;

//数据类型isXXX特化
template <typename TElem, typename TDevice>
constexpr bool IsScalar<Scalar<TElem, TDevice> > = true;

template <typename TElem, typename TDevice>
constexpr bool IsMatrix<Matrix<TElem, TDevice> > = true;


//建立具体类型和类别标签之间的关联
template <typename T>
struct DataCategory_
{
private:
    template <bool isScalar, bool IsMatrix,
        bool IsBatchScalar, bool IsBatchMatrix,
        typename TDummy=void
    >
    struct helper;

    template <typename TDummy>
    struct helper<true, false, false, false, TDummy>
    {
        using type = CategoryTags::Scalar;
    };

    template <typename TDummy>
    struct helper<false, true, false, false, TDummy>
    {
        using type = CategoryTags::Matrix;
    };

    template <typename TDummy>
    struct helper<false, false, true, false, TDummy>
    {
        using type = CategoryTags::BatchScalar;
    };    

    template <typename TDummy>
    struct helper<false, false, false, true, TDummy>
    {
        using type = CategoryTags::BatchMatrix;
    };
public:
    using type = typename 
    helper<IsScalar<T>, IsMatrix<T>, IsBatchScalar<T>, IsBatchMatrix<T> >::type;

    template <typename T>
    using DataCategory = typename DataCategory_<T>::type;
};

//PrincipalDataType获取主体类型
template <typename TCategory, typename TElem, typename TDevice>
struct PrincipalDataType_;

template <typename TElem, typename TDevice>
struct PrincipalDataType_<CategoryTags::Scalar, TElem, TDevice>
{
    using type = Scalar<TElem, TDevice>;
};

template <typename TElem, typename TDevice>
struct PrincipalDataType_<CategoryTags::Matrix, TElem, TDevice>
{
    using type = Matrix<TElem, TDevice>;
};

template <typename TElem, typename TDevice>
struct PrincipalDataType_<CategoryTags::BatchScalar, TElem, TDevice>
{
    using type = BatchScalar<TElem, TDevice>;
};

template <typename TElem, typename TDevice>
struct PrincipalDataType_<CategoryTags::BatchMatrix, TElem, TDevice>
{
    using type = BatchMatrix<TElem, TDevice>;
};

template <typename TCategory, typename TElem, typename TDevice>
using PrincipalDataType = typename PrincipalDataType_<TCategory, TElem, TDevice>::type;

//底层数据访问接口
template <typename TData>
struct LowerAccessImpl;

template <typename TData>
auto LowerAccess(TData&& p)
{
    using RawType = RemConstRef<TData>;
    return LowerAccessImpl<RawType>(std::forward<TData>(p));
}
}
#endif