#ifndef TRIVIALMATRIX_HPP_
#define TRIVIALMATRIX_HPP_

//平凡矩阵实现，矩阵元素全为一个值
#include <assert.h>
#include <METANN/Base/Device.hpp>
#include <METANN/Base/DataCategory.hpp>
#include <METANN/Memory/ContinuesMemories.hpp>

namespace METANN
{
template <typename TElem, typename TDevice, typename TScalar>
class TrivialMatrix
{
public:
    using ElementType = TElem;
    using DeviceType = TDevice;
public:
    TrivialMatrix(size_t p_rowNum, size_t p_colNum, TScalar p_val):
    m_rowNum(p_rowNum), m_colNum(p_colNum), m_val(p_val){}

    TrivialMatrix(size_t p_rowNum=0, size_t p_colNum=0):
    m_rowNum(p_rowNum), m_colNum(p_colNum), 
    m_val(std::move(TScalar<TElem, TDevice>(std::move(TElem()))))
    {}    
    size_t RowNum() const{return m_rowNum;}
    size_t ColNum() const{return m_colNum;}

    //读访问接口
    auto ElementValue() const {return m_val;}

    //求值接口
    bool operator==(const TrivialMatrix& val) const;
    template <typename OtherType>
    bool operator==(const OtherType& val) const;
    template <typename TData>
    bool operator!=(const TData& val) const;

    //求值注册
    auto EvalRegister() const;    
private:
    TScalar m_val;
    size_t m_rowNum;
    size_t m_colNum;
    //求值结果缓存

};

template <typename TElem, typename TDevice, typename TScalar>
constexpr bool IsMatrix<TrivialMatrix<TElem, TDevice, TScalar>> = true;

template <typename TElem, typename TDevice, typename TVal>
auto MakeTrivialMatrix(size_t rowNum, size_t colNum, TVal&& val)
{
    using RawVal = RemConstRef<Tval>;
    //val本身为标量类型，需要检查设备类型
    if constexpr (IsScalar<RawVal>)
    {
        //传入值类型与TrivialMatrix相同或者设备为CPU
        static_assert(std::is_same<typename RawVal::DeviceType, DeviceTags::CPU>::value || 
                        std::is_same<typename RawVal::DeviceType, TDevice>::value);
        
        return TrivialMatrix<TElem, TDevice, RawVal>(rowNum, colNum, val);
    }
    //新建一个设备为CPU的标量构成平凡矩阵
    else
    {
        TElem elem = static_cast<TElem>(val);
        Scalar<TElem, DeviceTags::CPU> scalar(std::move(val));
        return TrivialMatrix<TElem, TDevice, decltype(scalar)>(rowNum, colNum, std::move(scalar));
    }
}
}

#endif