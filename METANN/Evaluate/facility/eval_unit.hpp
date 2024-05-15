#pragma once

#include <unordered_map>
#include <vector>
#include <METANN/Base/DataCategory.hpp>
#include <iostream>

namespace METANN
{
template <typename TDevice>
class BaseEvalUnit
{
public:
    using DeviceType = TDevice;
    virtual ~BaseEvalUnit()=default;
    virtual void Eval() = 0;
};

template <>
class BaseEvalUnit<DeviceTags::CPU>
{
public:
    using DeviceType = DeviceTags::CPU;
    virtual ~BaseEvalUnit()=default;
    virtual void Eval(){}
};
}
