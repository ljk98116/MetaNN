#ifndef OP_TRANSPOSE_HPP_
#define OP_TRANSPOSE_HPP_

#include <type_traits>
#include <METANN/Operators/operators.hpp>
#include <METANN/Operators/facility/organizer.hpp>

namespace METANN
{

template <typename TP>
class OperTranspose_
{
private:
    using rawM = RemConstRef<TP>;
public:
    static constexpr bool valid = IsMatrix<rawM> || IsBatchMatrix<rawM>;
public:
    static auto Eval(TP&& p_m)
    {
        using ResType = UnaryOp<UnaryOpTags::Transpose, rawM>;
        return ResType(std::forward<TP>(p_m));
    }
};

template <typename TP>
auto Transpose(TP&& p_m)
{
    return OperTranspose_<TP>::Eval(std::forward<TP>(p_m));
}

//特化OperOrganizer
template <>
class OperOrganizer<UnaryOpTags::Transpose, CategoryTags::Matrix>
{
private:
    template <typename THead, typename ...TRemain>
    bool SameDim(const THead&, const TRemain&...)
    {
        return true;
    }
    template <typename THead, typename TCur, typename ...TRemain>
    bool SameDim(const THead& head, const TCur& cur, const TRemain&... rem)
    {
        const bool tmp = (head.RowNum() == cur.RowNum()) && (head.ColNum() == cur.ColNum());
        return tmp && SameDim(cur, rem...);
    }    
public:
    template <typename THead, typename...TRemain>
    OperOrganizer(const THead& head, const TRemain&... rem):
    m_rowNum(head.ColNum()), m_colNum(head.RowNum())
    {
        assert(SameDim(head, rem...));
    }    
    size_t RowNum() const{return m_rowNum;}
    size_t ColNum() const{return m_colNum;}
protected:
    size_t m_rowNum;
    size_t m_colNum;
};

template <>
class OperOrganizer<UnaryOpTags::Transpose, CategoryTags::BatchMatrix> :
public OperOrganizer<UnaryOpTags::Transpose, CategoryTags::Matrix>
{
    using BaseType = OperOrganizer<UnaryOpTags::Transpose, CategoryTags::Matrix>;
public:
    template <typename TData>
    OperOrganizer(const TData& data):
    BaseType(data), m_batchNum(data.BatchNum())
    {}
private:
    size_t m_batchNum;
};

}

#endif