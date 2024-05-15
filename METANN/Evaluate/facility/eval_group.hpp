#pragma once

#include <METANN/Evaluate/facility/eval_unit.hpp>
#include <list>
#include <memory>

namespace METANN
{

template <typename TDevice>
class BaseEvalGroup
{
public:
    virtual ~BaseEvalGroup()=default;
    virtual std::shared_ptr<BaseEvalUnit<TDevice> > GetEvalUnit() = 0;
    virtual void Merge(BaseEvalUnit<TDevice>&) = 0;
    virtual void Merge(BaseEvalUnit<TDevice>&&) = 0;
};

template <typename TEvalUnit>
class TrivialEvalGroup : public BaseEvalGroup<typename TEvalUnit::DeviceType>
{
    using DeviceType = typename TEvalUnit::DeviceType;
public:
    std::shared_ptr<BaseEvalUnit<DeviceType> > GetEvalUnit() override
    {
        std::shared_ptr<BaseEvalUnit<DeviceType> > res;
        if(!m_unitList.empty())
        {
            res = std::make_shared<TEvalUnit>(std::move(m_unitList.front()));
            m_unitList.pop_front();
        }
        return res;
    }

    void Merge(BaseEvalUnit<DeviceType>& unit) override
    {
        m_unitList.push_back(static_cast<TEvalUnit&>(unit));
    }
    void Merge(BaseEvalUnit<DeviceType>&& unit) override
    {
        m_unitList.push_back(static_cast<TEvalUnit&&>(unit));
    }
private:
    std::list<TEvalUnit> m_unitList;
};

}