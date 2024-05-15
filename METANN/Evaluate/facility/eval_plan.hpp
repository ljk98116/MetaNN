#pragma once

#include <list>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>

#include <METANN/Evaluate/facility/eval_pool.hpp>
#include <METANN/Evaluate/facility/eval_unit.hpp>
#include <METANN/Evaluate/facility/eval_handle.hpp>
#include <METANN/Evaluate/facility/eval_group.hpp>

#include <METANN/Evaluate/cpu/trivial_eval_pool.hpp>

namespace METANN
{

namespace NSEvalPlan
{
//找深度最大的操作对应的深度
inline size_t OperandDepth(std::unordered_map<const void*, size_t>& depMap, const std::vector<const void*>& paramPtr)
{
    int res = -1;
    for(auto p : paramPtr)
    {
        auto it = depMap.find(p);
        if(it != depMap.end()) res = std::max(res, (int)it->second);
    }
    return (size_t)res;
}
}

template <typename TDevice>
using EvalCluster = std::unordered_map<std::type_index, std::shared_ptr<BaseEvalGroup<TDevice>>>;

template <typename TDevice>
class EvalLayer
{
public:
    size_t Size() const
    {
        return m_evalSeq.size();
    }
    EvalCluster<TDevice>& operator[](size_t id)
    {
        return m_evalSeq[id];
    }
    bool Empty() const
    {
        return m_evalSeq.empty();
    }
    void Clear()
    {
        m_evalSeq.clear();
        m_operands.clear();
        m_outputs.clear();
    }
    template <typename TEvalGroup, typename TEvalUnit>
    void EvalRegister(TEvalUnit&& evalReq, const void* resPtr, const std::vector<const void*>& paramPtr)
    {
        //结果指针为空，不需要求值
        if(!resPtr) return;
        //结果指针已存在，不需要求值
        if(m_outputs.find(resPtr) != m_outputs.end()) return;

        size_t depth = NSEvalPlan::OperandDepth(m_outputs, paramPtr) + 1;
        if(m_evalSeq.size() <= depth)
        {
            m_evalSeq.resize(depth + 1);
        }
        EvalCluster<TDevice>& ec = m_evalSeq[depth];
        const auto typeindex = std::type_index(typeid(TEvalGroup));
        auto it = ec.find(typeindex);
        if(it == ec.end())
        {
            it = ec.insert({typeindex, std::make_shared<TEvalGroup>()}).first;
        }
        it->second->Merge(std::forward<TEvalUnit>(evalReq));
        m_outputs.insert({resPtr, depth});
    }
private:
    std::vector<EvalCluster<TDevice> > m_evalSeq;
    std::unordered_set<const void*> m_operands;
    std::unordered_map<const void*, size_t> m_outputs;
};

template <typename TDevice>
class EvalPlan
{
private:
    //全局EvalPool类型，单例，线程无关，所有线程的Pool均为该类型
    static EvalPoolEnum& GlobalEvalPool()
    {
        static EvalPoolEnum inst = EvalPoolEnum::Trivial;
        return inst;
    }
    //线程级EvalPool类型，单例，线程相关，在每个线程上存储静态的Pool类型
    static EvalPoolEnum& ThreadEvalPool()
    {
        static thread_local EvalPoolEnum inst = EvalPoolEnum::Trivial;
        return inst;
    }
    static EvalPlan& ThreadInst()
    {
        static thread_local EvalPlan inst;
        return inst;
    }
public:
    static void SetEvalPool(EvalPoolEnum epType)
    {
        GlobalEvalPool() = epType;
    }
    template <typename TEvalGroup, typename TEvalUnit>
    static void Register(TEvalUnit&& evalReq, const void* outputPtr, const std::vector<const void*>& paramPtr)
    {
        ThreadInst().template EvalRegister<TEvalGroup>(std::forward<TEvalUnit>(evalReq), outputPtr, paramPtr);
    }
    static void Eval()
    {
        EvalPlan& plan = ThreadInst();
        //当前线程的pool实例和全局pool实例不同
        //当前线程的pool是全局的pool
        if(ThreadEvalPool() != GlobalEvalPool() || !plan.m_evalPool)
        {
            switch(GlobalEvalPool())
            {
                case EvalPoolEnum::Trivial:
                {
                    plan.m_evalPool = &(TrivialEvalPool<TDevice>::Instance());
                    break;
                }
                default:
                    assert(false);
            }
            ThreadEvalPool() = GlobalEvalPool();
        }
        if(!plan.m_evalPool)
        {
            throw std::runtime_error("No EvalPool is available.");
        }
        //本线程的plan对当前层求值
        plan.DoLayerEval();
    }
private:
    EvalPlan() : m_evalPool(nullptr)
    {
        m_evalLayers.resize(1);
    }
    //取最后一层进行求值
    template <typename TEvalGroup, typename TEvalUnit>
    void EvalRegister(TEvalUnit&& evalReq, const void *outputPtr, const std::vector<const void *>& paramPtr)
    {
        auto& curLayer = m_evalLayers.back();
        curLayer.template EvalRegister<TEvalGroup>(std::forward<TEvalUnit>(evalReq), outputPtr, paramPtr);
    }
    void DoLayerEval()
    {
        EvalLayer<TDevice>& curLayer = m_evalLayers.back();
        if(curLayer.Empty()) return;

        m_evalLayers.push_back(EvalLayer<TDevice>{});
        size_t seqLen = curLayer.Size();

        //对本层所有EvalCluster求值
        for(size_t i=0;i<seqLen;++i)
        {
            EvalCluster<TDevice>& ec = curLayer[i];
            for(auto& eg : ec)
            {
                while(auto unit = eg.second->GetEvalUnit())
                {
                    m_evalPool->Process(unit);
                }
            }
            m_evalPool->Barrier();
            //递归求值，上一层还有需要求值的内容
            if(!m_evalLayers.back().Empty())
            {
                DoLayerEval();
            }
        }
        //本层计算完成
        m_evalLayers.pop_back();
        curLayer.Clear();
    }
private:
    std::list<EvalLayer<TDevice> > m_evalLayers;
    BaseEvalPool<TDevice>* m_evalPool;
};

}