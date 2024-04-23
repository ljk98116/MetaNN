#include <METANN/NSVarTypeDict/VarTypeDict.hpp>
#include <METANN/NSVarTypeDict/Policy.hpp>

#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"

using namespace NSVarTypeDict;

struct A;
struct B;
struct C;
struct Weight;

struct FParams : public VarTypeDict<A, B, Weight>{};

template <typename TIn>
double fn(const TIn& tin)
{
    auto a = tin.template Get<A>();
    auto b = tin.template Get<B>();
    auto w = tin.template Get<Weight>();
    return a ? b : w;
}

TEST_GROUP(VarTypeTestGroup)
{

};

TEST(VarTypeTestGroup, FParamTest)
{
    auto res = fn(FParams::Create().Set<A>(true).Set<B>(2.4).Set<Weight>(0.1));
    CHECK(res == 2.4);
    auto fParams = FParams::Create().Set<A>(true).Set<B>(2.4).Set<Weight>(0.1);
    CHECK_EQUAL(fParams.template Get<A>(), true);
    CHECK_EQUAL(fParams.template Get<B>(), 2.4);
    CHECK_EQUAL(fParams.template Get<Weight>(), 0.1);

    //addItem, delItem
    using MyDict = VarTypeDict<A, B>;
    using DictWithMoreItems = addItem<MyDict, C>;
    static_assert(boost::mpl::equal<DictWithMoreItems, VarTypeDict<A, B, C>>::value);

    using DictWithLessItems = delItem<MyDict, A>;
    static_assert(boost::mpl::equal<DictWithLessItems, VarTypeDict<B>>::value);
};

TEST_GROUP(PolicyTestGroup)
{

};

TEST(PolicyTestGroup, PolicyTest)
{
    int a[] = {1, 2, 3, 4, 5};
    std::cerr << Accumulator<>::Eval(a) << std::endl;
    std::cerr << Accumulator<PMulAccu>::Eval(a) << std::endl;
    std::cerr << Accumulator<PMulAccu, PAve>::Eval(a) << std::endl;
    std::cerr << Accumulator<PAve, PMulAccu>::Eval(a) << std::endl;
    std::cerr << Accumulator<PAve, PMulAccu, PValueTypeIs<double> >::Eval(a) << std::endl;
}

int main(int argc, char *argv[])
{
    //FParams::Create().Set<A>(true);
    CommandLineTestRunner::RunAllTests(argc, argv);
    return 0;
}