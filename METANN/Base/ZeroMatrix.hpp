#ifndef ZEROMATRIX_HPP_
#define ZEROMATRIX_HPP_

#include <assert.h>
#include <METANN/Base/Device.hpp>
#include <METANN/Base/DataCategory.hpp>
#include <METANN/Memory/ContinuesMemories.hpp>

namespace METANN
{
template <typename TElem, typename TDevice, typename TScalar>
class ZeroMatrix
{
public:
    using ElementType = TElem;
    using DeviceType = TDevice;
public:
    ZeroMatrix(size_t p_rowNum=0, size_t p_colNum=0):
    m_rowNum(p_rowNum), m_colNum(p_colNum)
    {}    
    size_t RowNum() const{return m_rowNum;}
    size_t ColNum() const{return m_colNum;}

    //求值接口
    bool operator==(const ZeroMatrix& val) const
    {
        return m_rowNum == val.m_rowNum && m_colNum == val.m_colNum;
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
    auto EvalRegister() const;

private:
    size_t m_rowNum;
    size_t m_colNum;                
};
}
#endif