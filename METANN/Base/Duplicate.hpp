#ifndef DUPLICATE_HPP_
#define DUPLICATE_HPP_

#include <cassert>
#include <METANN/Base/DataCategory.hpp>

//具有复制意义的列表,比如矩阵A，[A,A,...,A],不支持写操作
namespace METANN
{
template <typename TData>
class Duplicate;

template <typename TData, typename TDataCate>
class DuplicateImp;

template <typename TData>
class DuplicateImp<TData, CategoryTags::Matrix>
{
public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;

    DuplicateImp(TData data, size_t batchNum): 
    m_data(std::move(data)), m_batchNum(batchNum)
    {
        assert(batchNum != 0);
    }
public:
    size_t RowNum() const {return m_data.RowNum();}
    size_t ColNum() const {return m_data.ColNum();}
    size_t BatchNum() const {return m_batchNum;}
    const TData& Element() const {return m_data;}

    bool operator== (const Duplicate<TData>& val) const
    {
        const DuplicateImp<TData, CategoryTags::Matrix>& tmp = static_cast<const DuplicateImp<TData, CategoryTags::Matrix>&>(val);
        return (tmp.m_data == m_data) && (tmp.m_batchNum == m_batchNum);
    }

    template <typename TOtherType>
    bool operator== (const TOtherType&) const
    {
        return false;
    }

    template <typename TCompData>
    bool operator!= (const TCompData& val) const
    {
        return !(operator==(val));
    }
protected:
    TData m_data;
    size_t m_batchNum;
};

template <typename TData>
class DuplicateImp<TData, CategoryTags::Scalar>
{
public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;

    DuplicateImp(TData data, size_t batchNum): 
    m_data(std::move(data)), m_batchNum(batchNum)
    {
        assert(batchNum != 0);
    }
public:
    size_t BatchNum() const {return m_batchNum;}
    const TData& Element() const {return m_data;}

    bool operator== (const Duplicate<TData>& val) const
    {
        const DuplicateImp<TData, CategoryTags::Matrix>& tmp = static_cast<const DuplicateImp<TData, CategoryTags::Matrix>&>(val);
        return (tmp.m_data == m_data) && (tmp.m_batchNum == m_batchNum);
    }

    template <typename TOtherType>
    bool operator== (const TOtherType&) const
    {
        return false;
    }

    template <typename TCompData>
    bool operator!= (const TCompData& val) const
    {
        return !(operator==(val));
    }
protected:
    TData m_data;
    size_t m_batchNum;    
};

template <typename TData>
class Duplicate : public DuplicateImp<TData, DataCategory<TData> >
{
public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;
    using DuplicateImp<TData, DataCategory<TData> >::DuplicateImp;
};

template <typename TData>
constexpr bool IsBatchMatrix<Duplicate<TData> > = IsMatrix<TData>;

template <typename TData>
constexpr bool IsBatchScalar<Duplicate<TData> > = IsScalar<TData>;

template <typename TData>
auto MakeDuplicate(size_t batchNum, TData&& data)
{
    using RawData = RemConstRef<TData>;
    return Duplicate<RawData>(std::forward<RawData>(data), batchNum);
}

template <typename TData, typename ...TParams>
auto MakeDuplicate(size_t batchNum, TParams&& ...params)
{
    using RawData = RemConstRef<TData>;
    //临时对象
    RawData tmp(std::forward<RawData>(params)...);
    //避免拷贝，使用move
    return Duplicate<RawData>(std::move(tmp), batchNum);
}

}

#endif