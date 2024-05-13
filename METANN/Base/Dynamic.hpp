#pragma once

#include <METANN/Base/Matrix.hpp>
#include <METANN/Base/Batch.hpp>

//DynamicCategory->DynamicWrapper
//DynamicData:[std::shared_ptr<DynamicCategory>]
namespace METANN
{

template <typename TElem, typename TDevice, typename TDataCate>
class DynamicCategory;

template <typename TElem, typename TDevice>
class DynamicCategory<TElem, TDevice, CategoryTags::Matrix>
{
public:
    using ElementType = TElem;
    using DeviceType = TDevice;
    using EvalType = PrincipalDataType<CategoryTags::Matrix, TElem, TDevice>;
public:
    template <typename TBase>
    DynamicCategory(const TBase& base):
    m_rowNum(base.RowNum()), m_colNum(base.ColNum())
    {}
    
    virtual ~DynamicCategory() = default;

    size_t RowNum() const {return m_rowNum;}
    size_t ColNum() const {return m_colNum;}

    //求值接口，纯虚函数
    virtual bool operator==(const DynamicCategory& val) const = 0;
    virtual bool operator!=(const DynamicCategory& val) const = 0; 
private:
    size_t m_rowNum;
    size_t m_colNum;
};

template <typename TElem, typename TDevice>
class DynamicCategory<TElem, TDevice, CategoryTags::BatchMatrix>
{
public:
    using ElementType = TElem;
    using DeviceType = TDevice;
    using EvalType = PrincipalDataType<CategoryTags::BatchMatrix, TElem, TDevice>;
public:
    template <typename TBase>
    DynamicCategory(const TBase& base):
    m_rowNum(base.RowNum()), m_colNum(base.ColNum()), m_batchNum(base.BatchNum())
    {}
    
    virtual ~DynamicCategory() = default;

    size_t RowNum() const {return m_rowNum;}
    size_t ColNum() const {return m_colNum;}
    size_t BatchNum() const {return m_batchNum;}

    //求值接口，纯虚函数
    virtual bool operator==(const DynamicCategory& val) const = 0;
    virtual bool operator!=(const DynamicCategory& val) const = 0;   
private:
    size_t m_rowNum;
    size_t m_colNum;
    size_t m_batchNum;
};

template <typename TBaseData>
class DynamicWrapper : 
public DynamicCategory<typename TBaseData::ElementType, typename TBaseData::DeviceType, DataCategory<TBaseData> >
{
    using TBase = DynamicCategory<typename TBaseData::ElementType, typename TBaseData::DeviceType, DataCategory<TBaseData> >;
public:
    DynamicWrapper(TBaseData baseData):
    TBase(baseData),
    m_baseData(std::move(baseData))
    {}

    bool operator==(const TBase& val) const override
    {
        try
        {
            const DynamicWrapper& tmp = dynamic_cast<const DynamicWrapper&>(val);
            return tmp.m_baseData == m_baseData;
        }
        catch(std::bad_cast&)
        {
            return false;
        }
    }
    bool operator!=(const TBase& val) const override
    {
        return !(operator==(val));
    }
    const TBaseData& BaseData() const {return m_baseData;}
private:
    TBaseData m_baseData;
};

template <typename TElem, typename TDevice, typename TDataCate>
class DynamicData;

template <typename TElem, typename TDevice>
class DynamicData<TElem, TDevice, CategoryTags::Matrix>
{
    using BaseData = DynamicCategory<TElem, TDevice, CategoryTags::Matrix>;
public:
    using ElementType = TElem;
    using DeviceType = TDevice;

    DynamicData()=default;
    template <typename OriData>
    DynamicData(std::shared_ptr<DynamicWrapper<OriData> > data)
    {
        m_baseData = std::move(data);
    }

    size_t RowNum() const {return m_baseData->RowNum();}
    size_t ColNum() const {return m_baseData->ColNum();}

    bool operator==(const DynamicData& val) const
    {
        if((!m_baseData) && (!val.m_baseData))
        {
            return true;
        }
        if((!m_baseData) || (!val.m_baseData))
        {
            return false;
        }
        BaseData& v1 = *m_baseData;
        BaseData& v2 = *val.m_baseData;
        return v1 == v2;
    }

    template <typename OtherType>
    bool operator==(const OtherType& val) const
    {
        return false;
    }

    bool operator!=(const DynamicData& val) const
    {
        return !(operator==(val));
    }

    template <typename T>
    const T* TypeCast() const
    {
        const BaseData *ptr = m_baseData.get();
        auto ptrCast = dynamic_cast<const DynamicWrapper<T>*>(ptr);
        return ptrCast ? &(ptrCast->BaseData()) : nullptr;
    }

    bool IsEmpty() const {return m_baseData == nullptr;}
private:
    std::shared_ptr<BaseData> m_baseData;
};

template <typename TElem, typename TDevice>
class DynamicData<TElem, TDevice, CategoryTags::BatchMatrix>
{
    using BaseData = DynamicCategory<TElem, TDevice, CategoryTags::BatchMatrix>;
public:
    using ElementType = TElem;
    using DeviceType = TDevice;

    DynamicData()=default;
    template <typename OriData>
    DynamicData(std::shared_ptr<DynamicWrapper<OriData> > data)
    {
        m_baseData = std::move(data);
    }

    size_t RowNum() const {return m_baseData->RowNum();}
    size_t ColNum() const {return m_baseData->ColNum();}
    size_t BatchNum() const {return m_baseData->BatchNum();}

    bool operator==(const DynamicData& val) const
    {
        if((!m_baseData) && (!val.m_baseData))
        {
            return true;
        }
        if((!m_baseData) || (!val.m_baseData))
        {
            return false;
        }
        BaseData& v1 = *m_baseData;
        BaseData& v2 = *val.m_baseData;
        return v1 == v2;
    }

    template <typename OtherType>
    bool operator==(const OtherType& val) const
    {
        return false;
    }
    
    bool operator!=(const DynamicData& val) const
    {
        return !(operator==(val));
    }

    template <typename T>
    const T* TypeCast() const
    {
        const BaseData *ptr = m_baseData.get();
        auto ptrCast = dynamic_cast<const DynamicWrapper<T>*>(ptr);
        return ptrCast ? &(ptrCast->BaseData()) : nullptr;
    }

    bool IsEmpty() const {return m_baseData == nullptr;}
private:
    std::shared_ptr<BaseData> m_baseData;
};

template <typename TData>
constexpr bool IsDynamic = false;

template <typename C, typename D, typename E>
constexpr bool IsDynamic<DynamicData<C, D, E>> = true;

template <typename C, typename D, typename E>
constexpr bool IsDynamic<DynamicData<C, D, E>& > = true;

template <typename C, typename D, typename E>
constexpr bool IsDynamic<DynamicData<C, D, E>&& > = true;

template <typename C, typename D, typename E>
constexpr bool IsDynamic<const DynamicData<C, D, E>& > = true;

template <typename C, typename D, typename E>
constexpr bool IsDynamic<const DynamicData<C, D, E>&& > = true;

template <typename TData>
auto MakeDynamic(TData&& data)
{
    //看是否为DynamicData类型
    if constexpr (IsDynamic<TData>)
    {
        return std::forward<TData>(data);
    }
    else
    {
        //原始数据类型
        using rawData = RemConstRef<TData>;
        //对原始数据动态包装
        using TDeriveData = DynamicWrapper<TData>;
        //shared_ptr指针
        auto baseData = std::make_shared<TDeriveData>(std::forward<TData>(data));
        return DynamicData<
            typename rawData::ElementType, 
            typename rawData::DeviceType,
            DataCategory<rawData>
        >(std::move(baseData));
    }
}

template <typename TElem, typename TDevice>
constexpr bool IsMatrix<DynamicData<TElem, TDevice, CategoryTags::Matrix>> = true;

template <typename TElem, typename TDevice>
constexpr bool IsBatchMatrix<DynamicData<TElem, TDevice, CategoryTags::BatchMatrix>> = true;

}