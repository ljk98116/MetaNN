#ifndef CONTINUESMEMORY_H_
#define CONTINUESMEMORY_H_

#include <METANN/Memory/Allocator.hpp>

namespace METANN
{
//Allocator的内存管理器
//管理对象对应类型的智能指针、对应的起始地址
//使用智能指针实现浅拷贝，目的是提高运行速度
template <typename TElem, typename TDevice>
class ContinuesMemory
{
    //对类型去引用
    static_assert(std::is_same<RemConstRef<TElem>, TElem>::value);
    using ElementType = TElem;
public:
    explicit ContinuesMemory(size_t p_size):
    m_mem(Allocator<TDevice>::template Allocate<ElementType>(p_size)),
    m_memStart(m_mem.get()){}

    ContinuesMemory(std::shared_ptr<ElementType> p_mem, ElementType *p_memStart):
    m_mem(std::move(p_mem)), m_memStart(p_memStart){}

    auto RawMemory() const {return m_memStart;}
    const std::shared_ptr<ElementType> SharedPtr() const
    {
        return m_mem;
    }

    bool operator==(const ContinuesMemory& val) const
    {
        return (m_mem == val.m_mem) && (m_memStart == val.m_memStart);
    }

    bool operator!=(const ContinuesMemory& val) const
    {
        return !(operator==(val));
    }

    size_t Use_count() const
    {
        return m_mem.use_count();
    }
private:
    std::shared_ptr<ElementType> m_mem;
    ElementType *m_memStart;    
};
}

#endif