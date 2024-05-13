#ifndef ARRAY_HPP_
#define ARRAY_HPP_

#include <METANN/Base/DataCategory.hpp>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <vector>

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
        return m_buffer.use_count() == 1;
    }

private:
    size_t m_rowNum;
    size_t m_colNum;
    std::shared_ptr<std::vector<TData> > m_buffer;
};

template <typename TData>
class ArrayImp<TData, CategoryTags::Scalar>
{
public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;

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