#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include <assert.h>
#include <METANN/Base/Device.hpp>
#include <METANN/Base/DataCategory.hpp>
#include <METANN/Memory/ContinuesMemories.hpp>
#include <METANN/Evaluate/facility/eval_handle.hpp>

namespace METANN
{
template <typename TElem, typename TDevice=DeviceTags::CPU>
class Matrix
{
    //LowerAccessImpl<Matrix<TElem, TDevice> >可以访问到Matrix的私有成员
    friend struct LowAccessImpl<Matrix<TElem, TDevice> >;
    friend class Batch<TElem, TDevice, CategoryTags::Matrix>;
public:
    using ElementType = TElem;
    using DeviceType = TDevice;
public:
    Matrix(size_t p_rowNum=0, size_t p_colNum=0):
    m_mem(p_rowNum * p_colNum),
    m_rowNum(p_rowNum),
    m_colNum(p_colNum),
    m_rowLen(p_colNum)
    {} 
    size_t RowNum() const{return m_rowNum;}
    size_t ColNum() const{return m_colNum;}

    //读写访问接口
    void SetValue(size_t p_rowId, size_t p_colId, ElementType val);
    const auto operator()(size_t p_rowId, size_t p_colId) const;
    bool AvailableForWrite() const;

    //子矩阵接口
    Matrix SubMatrix(size_t p_rowB, size_t p_rowE, size_t p_colB, size_t p_colE) const;

    //求值接口
    bool operator==(const Matrix& val) const
    {
        return m_mem == val.m_mem && 
            m_rowNum == val.m_rowNum &&
            m_colNum == val.m_colNum &&
            m_rowLen == val.m_rowLen;
    }
    template <typename OtherType>
    bool operator==(const OtherType& val) const
    {
        return false;
    }
    template <typename TData>
    bool operator!=(const TData& val) const
    {
        return !(operator==(val));
    }
    //求值注册
    auto EvalRegister() const
    {
        return MakeConstEvalHandle(*this);
    }
private:
    Matrix(std::shared_ptr<ElementType> p_mem, ElementType *p_memStart,
        size_t p_rowNum, size_t p_colNum, size_t p_rowLen):
        m_mem(std::move(ContinuesMemory(p_mem, p_memStart))),
        m_rowNum(p_rowNum), m_colNum(p_colNum), m_rowLen(p_rowLen){}

private:
    ContinuesMemory<ElementType, DeviceType> m_mem;
    size_t m_rowNum;//行数
    size_t m_colNum;//列数
    size_t m_rowLen;//矩阵单行长度
};

//获取对应位置的矩阵值
template <typename TElem, typename TDevice>
const auto Matrix<TElem, TDevice>::operator()(size_t p_rowId, size_t p_colId) const
{
    assert((p_rowId < m_rowNum) && (p_colId < m_colNum));
    return (m_mem.RawMemory())[p_rowId * m_rowLen + p_colId];
}

//引用数为1才可写
template <typename TElem, typename TDevice>
bool Matrix<TElem, TDevice>::AvailableForWrite() const
{
    return m_mem.Use_count() == 1;
}

template <typename TElem, typename TDevice>
void Matrix<TElem, TDevice>::SetValue(size_t p_rowId, size_t p_colId, ElementType val)
{
    assert((p_rowId < m_rowNum) && (p_colId < m_colNum));
    assert(AvailableForWrite());
    (m_mem.RawMemory())[p_rowId * m_rowLen + p_colId] = val;
}

template <typename TElem, typename TDevice>
Matrix<TElem, TDevice> 
Matrix<TElem, TDevice>::SubMatrix(size_t p_rowB, size_t p_rowE, size_t p_colB, size_t p_colE) const
{
    assert((p_rowB < m_rowNum) && (p_rowE < m_rowNum));
    assert((p_colB <= m_colNum) && (p_colE <= m_colNum));
    ElementType *pos = m_mem.RawMemory() + p_rowB * m_rowLen + p_colB;
    return Matrix(
        m_mem.SharedPtr(),
        pos,
        p_rowE - p_rowB,
        p_colE - p_colB,
        m_rowLen
    );
}

//CPU上矩阵底层数据访问接口实现,特化
template <typename TElem>
struct LowAccessImpl<Matrix<TElem, DeviceTags::CPU> >
{
    LowAccessImpl(Matrix<TElem, DeviceTags::CPU> p): m_matrix(p){}
    //安全性存在问题，用户使用此处指针进行写操作会绕过UseCount检查
    auto MutableRawMemory(){return m_matrix.m_mem.RawMemory();}
    const auto RawMemory() const{return m_matrix.m_mem.RawMemory();}
    size_t RowLen() const{return m_matrix.m_rowLen;}
private:
    Matrix<TElem, DeviceTags::CPU> m_matrix;
};

}
#endif