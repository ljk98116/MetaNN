#pragma once

#include <METANN/Evaluate/facility/eval_unit.hpp>
#include <memory>

namespace METANN
{
enum class EvalPoolEnum
{
    Trivial
};

template <typename TDevice>
class BaseEvalPool
{
public:
    virtual ~BaseEvalPool()=default;
    virtual void Process(std::shared_ptr<BaseEvalUnit<TDevice> >&) = 0;
    virtual void Barrier() = 0;
};

template <typename TDevice>
class TrivialEvalPool;

}