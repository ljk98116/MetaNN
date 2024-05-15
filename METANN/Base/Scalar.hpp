#ifndef SCALAR_HPP_
#define SCALAR_HPP_

#include <METANN/Base/Device.hpp>
#include <METANN/Base/DataCategory.hpp>
#include <METANN/Evaluate/facility/eval_handle.hpp>

namespace METANN
{
//标量数据类型

//默认为CPU特化版本
template <typename TElem, typename TDevice=DeviceTags::CPU>
class Scalar
{
public:
    using ElementType = TElem;
    using DeviceType = TDevice;
public:
    Scalar(ElementType elem = ElementType()) : m_elem(elem){}
    auto& Value() {return m_elem;}
    auto Value() const {return m_elem;}

    //求值接口
    bool operator==(const Scalar& val) const
    {
        return m_elem == val.m_elem;
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
    ElementType m_elem;
};

}
#endif