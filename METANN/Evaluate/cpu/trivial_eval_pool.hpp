#pragma once

#include <METANN/Evaluate/facility/eval_pool.hpp>
#include <METANN/Base/Device.hpp>

namespace METANN
{
//平凡EvalPool对传入的单个Unit进行求值
//可使用类似线程池进行求值优化
template <>
class TrivialEvalPool<DeviceTags::CPU> : public BaseEvalPool<DeviceTags::CPU>
{
public:
    static TrivialEvalPool& Instance()
    {
        static TrivialEvalPool inst;
        return inst;
    }
private:
    TrivialEvalPool()=default;
public:
    //具体求值过程
    void Process(std::shared_ptr<BaseEvalUnit<DeviceTags::CPU> >& eu) override
    {
        eu->Eval();
    }
    //等待求值操作完成
    void Barrier() override{}
};

}