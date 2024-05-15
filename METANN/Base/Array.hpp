#ifndef ARRAY_HPP_
#define ARRAY_HPP_

#include <stdexcept>
#include <cassert>
#include <memory>
#include <vector>

#include <METANN/Base/DataCategory.hpp>
#include <METANN/Base/Device.hpp>
#include <METANN/Base/Batch.hpp>
#include <METANN/Evaluate/facility/eval_unit.hpp>
#include <METANN/Evaluate/facility/eval_handle.hpp>
#include <METANN/Evaluate/facility/eval_buffer.hpp>
#include <METANN/Evaluate/facility/eval_plan.hpp>
#include <METANN/Evaluate/facility/eval_group.hpp>

namespace METANN
{

template <typename TData, typename TDataCate>
class ArrayImp;

template <typename TData>
class Array : public ArrayImp<TData, DataCategory<TData> >
{
public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;
    using ArrayImp<TData, DataCategory<TData> >::ArrayImp;
};

template <typename TData>
constexpr bool IsBatchScalar<Array<TData>> = IsScalar<TData>;

template <typename TData>
constexpr bool IsBatchMatrix<Array<TData>> = IsMatrix<TData>;

namespace NSArray
{

template <typename TInputElem, typename TElem, typename TDevice, typename TCategory>
struct EvalUnit;

template <typename TInputElem, typename TElem>
struct EvalUnit<TInputElem, TElem, DeviceTags::CPU, CategoryTags::Matrix> : public BaseEvalUnit<DeviceTags::CPU>
{
    using ElementType = TElem;
    using DeviceType = DeviceTags::CPU;
public:
    EvalUnit(std::vector<TInputElem> p_input, 
    EvalHandle<Batch<TElem, DeviceTags::CPU, CategoryTags::Matrix > > p_handle):
    m_inputs(std::move(p_input)),
    m_evalHandle(std::move(p_handle))
    {}

    void Eval() override
    {
        if(m_inputs.empty()) m_evalHandle.Allocate(0, 0, 0);
        else
        {
            size_t tbn = m_inputs.size();
            size_t trn = m_inputs[0].Data().RowNum();
            size_t tcn = m_inputs[0].Data().ColNum();
            auto& res = m_evalHandle.MutableData();
            m_evalHandle.Allocate(tbn, trn, tcn);
            for(size_t bn=0;bn<tbn;++bn)
            {
                const auto& input = m_inputs[bn].Data();
                for(size_t i=0;i<trn;++i)
                for(size_t j=0;j<tcn;++j)
                {
                    res.SetValue(bn, i, j, input(i, j));
                }
            }
        }
        m_evalHandle.SetEval();
        //std::cout << "Evaluated" << std::endl;
    }
private:
    std::vector<TInputElem> m_inputs;
    EvalHandle<Batch<TElem, DeviceTags::CPU, CategoryTags::Matrix > > m_evalHandle;
};

}

template <typename TData>
class ArrayImp<TData, CategoryTags::Matrix>
{
public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;

    ArrayImp(size_t rowNum=0, size_t colNum=0): 
    m_rowNum(rowNum), m_colNum(colNum), m_buffer(new std::vector<TData>())
    {}

    template <typename TIterator, std::enable_if_t<IsIterator<TIterator> >* = nullptr>
    ArrayImp(TIterator b, TIterator e):
    m_rowNum(0), m_colNum(0), m_buffer(new std::vector<TData>(b, e))
    {
        const auto& buffer = *m_buffer;
        //检查数组内矩阵形状是否一致
        if(!buffer.empty())
        {
            m_rowNum = buffer[0].RowNum();
            m_colNum = buffer[0].ColNum();
            for(auto& mat : buffer)
            {
                if(mat.RowNum() != m_rowNum || mat.ColNum() != m_colNum)
                {
                    throw std::runtime_error("Dimension mismatch");
                }
            }
        }
    }

public:
    size_t RowNum() const {return m_rowNum;}
    size_t ColNum() const {return m_colNum;}
    size_t BatchNum() const {return m_buffer->size();}
    size_t size() const {return m_buffer->size();}
    //STL接口
    //push_back,size等
    //push_back需要进行拷贝
    void push_back(TData mat)
    {
        assert(AvailableForWrite());
        if(mat.RowNum() != m_rowNum || mat.ColNum() != m_colNum)
        {
            throw std::runtime_error("Dimension dismatch");
        }
        //注意使用emplace_back传入矩阵，调用TData的构造函数，原地加入，不进行内存拷贝
        m_buffer->emplace_back(std::move(mat));
    }

    //参数列表为右值引用,因为参数列表中可能有类，此时可以避免在函数内拷贝这些类
    //引用折叠，是指编译器实例化模板函数时对函数参数类型进行的计算，
    //实际函数调用时参数变量类型的变化仍然是右值变左值，左值仍然左值
    template <typename ...Args>
    void emplace_back(Args&&... args)
    {
        //参数列表传入函数后，在栈空间分配值，此时右值转变为具名变量，成为左值
        assert(AvailableForWrite());
        //使用std::forward保持右值属性避免拷贝
        TData tmp(std::forward<Args>(args)...);
        if(tmp.RowNum() != m_rowNum || tmp.ColNum() != m_colNum)
        {
            throw std::runtime_error("Dimension dismatch");
        }
        //注意使用emplace_back传入矩阵，调用TData的构造函数，原地加入，不进行内存拷贝
        m_buffer->emplace_back(std::move(tmp));        
    }

    void reserve(size_t num)
    {
        assert(AvailableForWrite());
        m_buffer.reserve(num);
    }

    void clear()
    {
        assert(AvailableForWrite());
        m_buffer.clear();
    }

    const auto& operator[] (size_t id) const
    {
        return (*m_buffer)[id];
    }
    
    auto& operator[] (size_t id)
    {
        return (*m_buffer)[id];
    }

    auto begin(){return m_buffer->begin();}
    auto begin() const{return m_buffer->begin();}
    auto end() {return m_buffer->end();}
    auto end() const{return m_buffer->end();}

    bool empty() const {return m_buffer->empty();}
    //求值接口
    bool operator==(const Array<TData>& val)
    {
        const ArrayImp<TData, CategoryTags::Matrix>& tmp = static_cast<ArrayImp<TData, CategoryTags::Matrix>&>(val);
        return m_buffer == tmp.m_buffer;
    }

    template <typename OtherType>
    bool operator==(const OtherType& val)
    {
        return false;
    }

    template <typename TCompdata>
    bool operator!=(const TCompdata& val)
    {
        return !(operator==(val));
    }

    bool AvailableForWrite() const
    {
        return (m_buffer.use_count() == 1) && (!m_evalBuffer.IsEvaluated());
    }

    auto EvalRegister() const
    {
        if(!m_evalBuffer.IsEvaluated())
        {
            //顶层求值结果句柄类型
            //需要去除常量、引用
            //std::declval<T>()生成T类型对象的右值引用，不实际构造对象
            using TopEvalHandle = std::decay_t<decltype(std::declval<TData>().EvalRegister())>;
            std::vector<TopEvalHandle> handleBuf;
            std::vector<const void*> depVec;
            //实际处理的顶层求值数量与数组大小相同
            handleBuf.reserve(this->size());
            depVec.reserve(this->size());
            //加入对应句柄和结果指针
            for(size_t i=0;i<this->size();++i)
            {
                handleBuf.push_back((*this)[i].EvalRegister());
                depVec.push_back(handleBuf.back().DataPtr());
            }

            auto outputHandle = m_evalBuffer.Handle();

            using EvalUnit = NSArray::EvalUnit<TopEvalHandle, ElementType, DeviceType, CategoryTags::Matrix>;
            using GroupType = TrivialEvalGroup<EvalUnit>;
            
            const void* dataPtr = outputHandle.DataPtr();
            EvalUnit unit(std::move(handleBuf), std::move(outputHandle));
            EvalPlan<DeviceType>::template Register<GroupType>(std::move(unit), dataPtr, std::move(depVec));
        }
        return m_evalBuffer.ConstHandle();
    }
private:
    size_t m_rowNum;
    size_t m_colNum;
    std::shared_ptr<std::vector<TData> > m_buffer;
    EvalBuffer<Batch<ElementType, DeviceType, CategoryTags::Matrix> > m_evalBuffer;
};

template <typename TData>
class ArrayImp<TData, CategoryTags::Scalar>
{
public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;

    //保留默认构造函数
    ArrayImp(size_t rowNum=0, size_t colNum=0): 
    m_buffer(new std::vector<TData>())
    {}
    template <typename TIterator, std::enable_if_t<IsIterator<TIterator> >* = nullptr>
    ArrayImp(TIterator b, TIterator e):
    m_buffer(new std::vector<TData>(b, e))
    {}
    size_t BatchNum() const {return m_buffer->size();}
    size_t size() const {return m_buffer->size();}
    //STL接口
    //push_back,size等
    //push_back需要进行拷贝
    void push_back(TData mat)
    {
        assert(AvailableForWrite());
        //注意使用emplace_back传入矩阵，调用TData的构造函数，原地加入，不进行内存拷贝
        m_buffer->emplace_back(std::move(mat));
    }

    //参数列表为右值引用,因为参数列表中可能有类，此时可以避免在函数内拷贝这些类
    //引用折叠，是指编译器实例化模板函数时对函数参数类型进行的计算，
    //实际函数调用时参数变量类型的变化仍然是右值变左值，左值仍然左值
    template <typename ...Args>
    void emplace_back(Args&&... args)
    {
        //参数列表传入函数后，在栈空间分配值，此时右值转变为具名变量，成为左值
        assert(AvailableForWrite());
        //使用std::forward保持右值属性避免拷贝
        TData tmp(std::forward<Args>(args)...);
        //注意使用emplace_back传入矩阵，调用TData的构造函数，原地加入，不进行内存拷贝
        m_buffer->emplace_back(std::move(tmp));        
    }

    void reserve(size_t num)
    {
        assert(AvailableForWrite());
        m_buffer.reserve(num);
    }

    void clear()
    {
        assert(AvailableForWrite());
        m_buffer.clear();
    }

    auto& operator[](size_t id)
    {
        return (*m_buffer)[id];
    }

    auto begin(){return m_buffer->begin();}
    auto begin() const{return m_buffer->begin();}
    auto end() {return m_buffer->end();}
    auto end() const{return m_buffer->end();}

    bool empty() const {return m_buffer->empty();}
    //求值接口
    bool operator==(const Array<TData>& val)
    {
        const ArrayImp<TData, CategoryTags::Scalar>& tmp = static_cast<ArrayImp<TData, CategoryTags::Scalar>&>(val);
        return m_buffer == tmp.m_buffer;
    }

    template <typename OtherType>
    bool operator==(const OtherType& val)
    {
        return false;
    }

    template <typename TCompdata>
    bool operator!=(const TCompdata& val)
    {
        return !(operator==(val));
    }
    bool AvailableForWrite() const
    {
        return m_buffer.use_count() == 1;
    }
private:
    std::shared_ptr<std::vector<TData> > m_buffer;
};

template <typename TIterator>
auto MakeArray(TIterator b, TIterator e)
{
    //提取迭代器值类型
    using TData = typename std::iterator_traits<TIterator>::value_type;
    using RawData = RemConstRef<TData>;

    return Array<RawData>(b, e);
}

}

#endif