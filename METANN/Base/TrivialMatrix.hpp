#ifndef TRIVIALMATRIX_HPP_
#define TRIVIALMATRIX_HPP_

//平凡矩阵实现，矩阵元素全为一个值
#include <assert.h>
#include <METANN/Base/Device.hpp>
#include <METANN/Base/DataCategory.hpp>
#include <METANN/Memory/ContinuesMemories.hpp>

#include <METANN/Evaluate/facility/eval_unit.hpp>
#include <METANN/Evaluate/facility/eval_buffer.hpp>
#include <METANN/Evaluate/facility/eval_group.hpp>
#include <METANN/Evaluate/facility/eval_plan.hpp>

namespace METANN
{

namespace NSTrivialMatrix
{
template <typename TElem, typename TDevice>
class EvalUnit;

template <typename TElem>
class EvalUnit<TElem, DeviceTags::CPU> : public BaseEvalUnit<DeviceTags::CPU>
{
public:
    template <typename TScaleElemType>
    EvalUnit(EvalHandle<Matrix<TElem, DeviceTags::CPU> > resBuf, 
            size_t rowNum, size_t colNum,
            const Scalar<TScaleElemType, DeviceTags::CPU>& val
    ):
    m_resHandle(std::move(resBuf)),
    m_rowNum(rowNum), m_colNum(colNum),
    m_val(val.Value())
    {}

    void Eval() override
    {
        //分配结果矩阵内存
        m_resHandle.Allocate(m_rowNum, m_colNum);
        auto& mutableData = m_resHandle.MutableData();
        auto lowLayer = LowAccess(mutableData);
        const size_t rowLen = lowLayer.RowLen();
        auto mem = lowLayer.MutableRawMemory();
        for(size_t i=0;i<m_rowNum;i++)
        {
            for(size_t j=0;j<m_colNum;j++)
            {
                mem[j] = m_val;
            }
            mem += rowLen;
        }
        //已经完成求值
        m_resHandle.SetEval();
    }
private:
    EvalHandle<Matrix<TElem, DeviceTags::CPU> > m_resHandle;
    size_t m_rowNum;
    size_t m_colNum;
    TElem m_val;
};

}

template <typename TElem, typename TDevice, typename TScalar>
class TrivialMatrix
{
public:
    using ElementType = TElem;
    using DeviceType = TDevice;
public:
    TrivialMatrix(size_t p_rowNum, size_t p_colNum, TScalar p_val):
    m_rowNum(p_rowNum), m_colNum(p_colNum), m_val(p_val){}

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
    auto EvalRegister() const
    {
        using TEvalUnit = NSTrivialMatrix::EvalUnit<ElementType, DeviceType>;
        using TEvalGroup = TrivialEvalGroup<TEvalUnit>;
        if(!m_evalBuf.IsEvaluated())
        {
            auto evalHandle = m_evalBuf.Handle();
            const void *outputPtr = evalHandle.DataPtr();
            TEvalUnit unit(std::move(evalHandle), m_rowNum, m_colNum, m_val);
            EvalPlan<DeviceType>::template Register<TEvalGroup>(std::move(unit), outputPtr, {});
        }
        return m_evalBuf.ConstHandle();
    }   
private:
    TScalar m_val;
    size_t m_rowNum;
    size_t m_colNum;
    //求值结果缓存
    EvalBuffer<Matrix<ElementType, DeviceType> > m_evalBuf;
};

template <typename TElem, typename TDevice, typename TScalar>
constexpr bool IsMatrix<TrivialMatrix<TElem, TDevice, TScalar>> = true;

template <typename TElem, typename TDevice, typename TVal>
auto MakeTrivialMatrix(size_t rowNum, size_t colNum, TVal&& val)
{
    using RawVal = RemConstRef<TVal>;
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