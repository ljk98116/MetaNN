#pragma once

#include <METANN/Facility/Policy.hpp>
#include <METANN/Base/Device.hpp>

namespace METANN
{

struct FeedBackPolicy
{
    using MajorClass = FeedBackPolicy;

    struct IsUpdateValueCate;
    struct IsFeedBackOutputValueCate;

    static constexpr bool IsUpdate = false;
    static constexpr bool IsFeedBack = false;
};

ValuePolicyObj(PUpdate, FeedBackPolicy, IsUpdate, true);
ValuePolicyObj(PNoUpdate, FeedBackPolicy, IsUpdate, false);
ValuePolicyObj(PFeedBack, FeedBackPolicy, IsFeedBackOutput, true);
ValuePolicyObj(PNoFeedBack, FeedBackPolicy, IsFeedBackOutput, false);

struct InputPolicy
{
    using MajorClass = InputPolicy;

    struct BatchModeValueCate;

    static constexpr bool BatchMode = false;
};
ValuePolicyObj(PBatchMode, InputPolicy, BatchMode, true);
ValuePolicyObj(PNoBatchMode, InputPolicy, BatchMode, false);

struct OperandPolicy
{
    using MajorClass = OperandPolicy;

    struct DeviceTypeCate : public METANN::DeviceTags {};
    using Device = DeviceTags::CPU;

    struct ElementTypeCate;
    using ElementType = float;
};

TypePolicyObj(PCPUDevice, OperandPolicy, Device, DeviceTags::CPU);
TypePolicyTemplate(PElementTypeIs, OperandPolicy, Element);


}