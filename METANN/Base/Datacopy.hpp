#pragma once

#include <METANN/Base/Matrix.hpp>
#include <stdexcept>
#include <cstring>

namespace METANN
{

template <typename TElem>
void DataCopy(const Matrix<TElem, DeviceTags::CPU>& src, const Matrix<TElem, DeviceTags::CPU>& dst)
{
    const size_t rownum = src.RowNum();
    const size_t colnum = src.ColNum();
    if(rownum != dst.RowNum() || colnum != dst.ColNum())
    {
        throw std::runtime_error("Matrix dimension dismatch");
    }
    const auto mem_src = LowerAccess(src);
    auto mem_dst = LowerAccess(dst);

    const size_t srcPackNum = mem_src.RowLen();
    const size_t dstPackNum = mem_dst.RowLen();

    const TElem* r1 = mem_src.RawMemory();
    TElem* r2 = mem_dst.MutableRawMemory();

    if((srcPackNum == colnum) && (dstPackNum == colnum))
    {
        memcpy(r2, r1, sizeof(TElem) * rownum * colnum);
    }
    else
    {
        for(size_t i=0;i<rownum;i++)
        {
            memcpy(r2, r1, sizeof(TElem) * colnum);
            r2 += dstPackNum;
            r1 += srcPackNum;
        }
    }
}

}