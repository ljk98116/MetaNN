#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <METANN/Base/Device.hpp>
#include <memory>

namespace METANN
{
template <typename DeviceTags>
struct Allocator;

template<>
struct Allocator<DeviceTags::CPU>
{
    template <typename TElem>
    static std::shared_ptr<TElem> Allocate(size_t pElemSize)
    {
        //可改进为高并发内存池操作，包括申请pElemSize个TElem以及内存池释放对应内存的动作
        return std::shared_ptr<TElem>(new TElem[pElemSize], [](TElem *nptr){delete[]nptr;});
    }

    
};

}
#endif