#ifndef BATCH_HPP_
#define BATCH_HPP_

//Batch中数据为连续存储
#include <METANN/Base/DataCategory.hpp>
#include <METANN/Memory/ContinuesMemories.hpp>

namespace METANN
{
template <typename TElement, typename TDevice, typename TCategory>
class Batch;

template <typename TElement, typename TDevice>
class Batch<TElement, TDevice, CategoryTags::Scalar>;

template <typename TElement, typename TDevice>
class Batch<TElement, TDevice, CategoryTags::Matrix>;

template <typename TElement, typename TDevice>
constexpr bool IsBatchMatrix<Batch<TElement, TDevice, CategoryTags::Matrix> > = true;

template <typename TElement, typename TDevice>
constexpr bool IsBatchScalar<Batch<TElement, TDevice, CategoryTags::Scalar> > = true;

//标量、矩阵的Batch特化
template <typename TElem, typename TDevice>
struct LowAccessImpl<Batch<TElem, TDevice, CategoryTags::Matrix>>;

//矩阵的Batch特化
template <typename TElement, typename TDevice>
class Batch<TElement, TDevice, CategoryTags::Matrix>
{
public:
    using ElementType = TElement;
    using DeviceType = TDevice;
    friend struct LowAccessImpl<Batch<TElement, TDevice, CategoryTags::Matrix> >;
public:
    Batch(size_t p_batchNum=0, size_t p_rowNum=0, size_t p_colNum=0):
    m_mem(p_batchNum * p_rowNum * p_colNum),
    m_rowNum(p_rowNum),
    m_colNum(p_colNum),
    m_rowLen(p_rowNum),
    m_rawMatrixSize(p_rowNum * p_colNum)
    {}

private:
    
private:
    ContinuesMemory<ElementType, DeviceType> m_mem;
    size_t m_rowNum;
    size_t m_colNum;
    size_t m_batchNum;
    size_t m_rowLen;
    size_t m_rawMatrixSize;
};

}
#endif