#ifndef OP_TRAITS_HPP
#define OP_TRAITS_HPP

//获取计算元素的类型和设备
namespace METANN
{
template <typename TOptag, typename TOp1, typename...TOperands>
struct OperElementType_
{
    using type = typename TOp1::ElementType;
}; 

template <typename TOptag, typename TOp1, typename...TOperands>
struct OperDeviceType_
{
    using type = typename TOp1::DeviceType;
};


}
#endif