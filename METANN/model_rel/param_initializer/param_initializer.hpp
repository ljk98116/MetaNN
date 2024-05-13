#pragma once

#include <map>
#include <string>
#include <METANN/Base/Matrix.hpp>
#include <METANN/Base/Datacopy.hpp>

namespace METANN
{

template <typename TElem, typename TPolicyCont, typename TFillers>
class Param_Initializer
{
public:
    using PolicyCont = TPolicyCont;
    Param_Initializer(TFillers&& filler):
    m_filler(std::move(filler))
    {}

    //给定Filler的Tag和值，在异类词典中添加对应的初始化器
    template <typename TTag, typename TVal>
    auto SetFiller(TVal&& val) &&
    {
        auto newfiller = m_filler.template Set<TTag, TVal>(std::forward<TVal>(val));
        using newfillerType = RemConstRef<decltype(newfiller)>;
        return Param_Initializer<TElem, TPolicyCont, newfillerType>(std::move(newfiller));
    }

    template <typename TTag>
    auto& GetFiller()
    {
        return m_filler.template Get<TTag>();
    }

    //获取/设置参数矩阵
    template <typename TElem2, typename TDevice2>
    void SetMatrix(const std::string& name, const Matrix<TElem2, TDevice2>& param)
    {
        if(m_params.find(param) != m_params.end())
        {
            throw std::runtime_error("Duplicate Parameter Matrix" + name);
        }
        //检查类型
        if constexpr (std::is_same<TElem, TElem2>::value && std::is_same<TDevice2, DeviceTags::CPU>::value)
        {
            m_params.insert({name, param});
        }
        //否则拷贝参数矩阵内容
        else
        {
            Matrix<TElem, DeviceTags::CPU> mat(param.RowNum(), param.ColNum());
            DataCopy(mat, param);
            m_params.insert({name, std::move(mat)});
        }
    }

    template <typename TElem2, typename TDevice2>
    void GetMatrix(const std::string& name, const Matrix<TElem2, TDevice2>& res) const
    {
        auto it = m_params.find(name);
        if(it == m_params.end())
        {
            throw std::runtime_error("Parameter not exist:" + name);
        }
        const auto& orinMat = it->second;
        if((res.RowNum() != orinMat.RowNum()) || (res.ColNum() != orinMat.ColNum()))
        {
            throw std::runtime_error("Matrix dimension not match: "+name);
        }
        //拷贝对应矩阵
        DataCopy(orinMat, res);
    }

    bool IsMatrixExist(const std::string& name) const
    {
        auto it = m_params.find(name);
        return it != m_params.end();
    }
private:
    TFillers m_filler;
    std::map<std::string, Matrix<TElem, DeviceTags::CPU> > m_params;
};

template <typename TElem, typename...TPolicies>
auto MakeInitializer()
{
    using npType = FillerTags2NamedParams<TPolicies...>;
    using FilDictType = RemConstRef<decltype(npType::Create())>;
    return Param_Initializer<TElem, TPolicies..., FilDictType>(npType::Create());
}


}