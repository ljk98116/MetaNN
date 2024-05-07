#ifndef OPTAGS_HPP_
#define OPTAGS_HPP_

namespace METANN
{
struct UnaryOpTags
{
    struct Sigmoid;
    struct Tanh;
    struct Transpose;

};

struct BinaryOpTags
{
    struct Add;
    struct Substract;
    struct ElementMul;
};

struct TernaryOpTags
{
    struct Interpolate;
    
};

}
#endif