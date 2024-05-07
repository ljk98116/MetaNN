#ifndef OP_OPERATORS_HPP_
#define OP_OPERATORS_HPP_

#include <METANN/Operators/facility/category_cal.hpp>
#include <METANN/Operators/facility/organizer.hpp>
#include <METANN/Operators/facility/traits.hpp>

namespace METANN
{

template <typename TOptag, typename TData>
class UnaryOp : public OperOrganizer<TOptag, OperCateCal<TOptag, TData> >
{
    //计算结果的类型
    using Cate = OperCateCal<TOptag, TData>;
public:
    using ElementType = typename OperElementType_<TOptag, TData>::type;
    using DeviceType = typename OperDeviceType_<TOptag, TData>::type;

public:
    UnaryOp(TData data):
    OperOrganizer<TOptag, Cate>(data),
    m_data(std::move(data))
    {}
    
    bool operator== (const UnaryOp& val) const
    {
        return m_data == val.m_data;
    }

    template <typename TOtherData>
    bool operator== (const TOtherData& val) const
    {
        return false;
    }

    template <typename TOtherData>
    bool operator!= (const TOtherData& val) const
    {
        return !(operator==(val));
    }

    const TData& Operand() const
    {
        return m_data;
    }
private:
    TData m_data;
    using TPrincipal = PrincipalDataType<Cate, ElementType, DeviceType>;
};

template <typename TOptag, typename TData1, typename TData2>
class BinaryOp : public OperOrganizer<TOptag, OperCateCal<TOptag, TData1, TData2> >
{
    //计算结果的类型
    using Cate = OperCateCal<TOptag, TData1, TData2>;
public:
    using ElementType = typename OperElementType_<TOptag, TData1, TData2>::type;
    using DeviceType = typename OperDeviceType_<TOptag, TData1, TData2>::type;

    BinaryOp(TData1 data1, TData2 data2):
    OperOrganizer<TOptag, Cate>(data1, data2),
    m_data1(std::move(data1)), m_data2(std::move(data2))
    {}
    
    bool operator== (const BinaryOp& val) const
    {
        return (m_data1 == val.m_data1) && (m_data2 == val.m_data2);
    }

    template <typename TOtherData>
    bool operator== (const TOtherData& val) const
    {
        return false;
    }

    template <typename TOtherData>
    bool operator!= (const TOtherData& val) const
    {
        return !(operator==(val));
    }

    const TData1& Operand1() const
    {
        return m_data1;
    }

    const TData2& Operand2() const
    {
        return m_data2;
    }    
private:
    TData1 m_data1;
    TData2 m_data2;
    using TPrincipal = PrincipalDataType<Cate, ElementType, DeviceType>;        
};

template <typename TOptag, typename TData1, typename TData2, typename TData3>
class TernaryOp : public OperOrganizer<TOptag, OperCateCal<TOptag, TData1, TData2, TData3> >
{
    //计算结果的类型
    using Cate = OperCateCal<TOptag, TData1, TData2, TData3>;
public:
    using ElementType = typename OperElementType_<TOptag, TData1, TData2, TData3>::type;
    using DeviceType = typename OperDeviceType_<TOptag, TData1, TData2, TData3>::type;

    TernaryOp(TData1 data1, TData2 data2, TData3):
    OperOrganizer<TOptag, Cate>(data1, data2, data3),
    m_data1(std::move(data1)), m_data2(std::move(data2)), m_data3(std::move(data3))
    {}
    
    bool operator== (const TernaryOp& val) const
    {
        return (m_data1 == val.m_data1) && (m_data2 == val.m_data2) && (m_data3 == val.m_data3);
    }

    template <typename TOtherData>
    bool operator== (const TOtherData& val) const
    {
        return false;
    }

    template <typename TOtherData>
    bool operator!= (const TOtherData& val) const
    {
        return !(operator==(val));
    }

    const TData1& Operand1() const
    {
        return m_data1;
    }

    const TData2& Operand2() const
    {
        return m_data2;
    }    

    const TData3& Operand3() const
    {
        return m_data3;
    }
private:
    TData1 m_data1;
    TData2 m_data2;
    TData3 m_data3;
    using TPrincipal = PrincipalDataType<Cate, ElementType, DeviceType>;        
};

//判断结果类型和对应类型是否一致即可，每一种操作4种情况
template <typename TOpTag, typename TData>
constexpr bool IsMatrix<UnaryOp<TOpTag, TData>>
    = std::is_same<OperCateCal<TOpTag, TData>, CategoryTags::Matrix>::value;
    
template <typename TOpTag, typename TData>
constexpr bool IsScalar<UnaryOp<TOpTag, TData>>
    = std::is_same<OperCateCal<TOpTag, TData>, CategoryTags::Scalar>::value;

template <typename TOpTag, typename TData>
constexpr bool IsBatchMatrix<UnaryOp<TOpTag, TData>>
    = std::is_same<OperCateCal<TOpTag, TData>, CategoryTags::BatchMatrix>::value;
    
template <typename TOpTag, typename TData>
constexpr bool IsBatchScalar<UnaryOp<TOpTag, TData>>
    = std::is_same<OperCateCal<TOpTag, TData>, CategoryTags::BatchScalar>::value;

template <typename TOpTag, typename TData1, typename TData2>
constexpr bool IsScalar<BinaryOp<TOpTag, TData1, TData2>>
    = std::is_same<OperCateCal<TOpTag, TData1, TData2>, CategoryTags::Scalar>::value;
    
template <typename TOpTag, typename TData1, typename TData2>
constexpr bool IsMatrix<BinaryOp<TOpTag, TData1, TData2>>
    = std::is_same<OperCateCal<TOpTag, TData1, TData2>, CategoryTags::Matrix>::value;

template <typename TOpTag, typename TData1, typename TData2>
constexpr bool IsBatchMatrix<BinaryOp<TOpTag, TData1, TData2>>
    = std::is_same<OperCateCal<TOpTag, TData1, TData2>, CategoryTags::BatchMatrix>::value;
    
template <typename TOpTag, typename TData1, typename TData2>
constexpr bool IsBatchScalar<BinaryOp<TOpTag, TData1, TData2>>
    = std::is_same<OperCateCal<TOpTag, TData1, TData2>, CategoryTags::BatchScalar>::value;

template <typename TOpTag, typename TData1, typename TData2, typename TData3>
constexpr bool IsScalar<TernaryOp<TOpTag, TData1, TData2, TData3>>
    = std::is_same<OperCateCal<TOpTag, TData1, TData2, TData3>, CategoryTags::Scalar>::value;
    
template <typename TOpTag, typename TData1, typename TData2, typename TData3>
constexpr bool IsMatrix<TernaryOp<TOpTag, TData1, TData2, TData3>>
    = std::is_same<OperCateCal<TOpTag, TData1, TData2, TData3>, CategoryTags::Matrix>::value;

template <typename TOpTag, typename TData1, typename TData2, typename TData3>
constexpr bool IsBatchMatrix<TernaryOp<TOpTag, TData1, TData2, TData3>>
    = std::is_same<OperCateCal<TOpTag, TData1, TData2, TData3>, CategoryTags::BatchMatrix>::value;
    
template <typename TOpTag, typename TData1, typename TData2, typename TData3>
constexpr bool IsBatchScalar<TernaryOp<TOpTag, TData1, TData2, TData3>>
    = std::is_same<OperCateCal<TOpTag, TData1, TData2, TData3>, CategoryTags::BatchScalar>::value;

}

#endif