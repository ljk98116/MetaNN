//SFINAE通过函数重载来避免报错
//典型的SFINAE实现方式：检测一个类是否有reserve成员
#include <type_traits>
#include <cstddef>
template <typename T>
struct has_reserve
{
    struct good{char a;};
    struct bad{char b[2];};

    //传入类型U，以及size_t*类型的U的成员
    template <typename U, size_t U::*>
    struct SFINAE{};

    template <typename U>
    static good reserve(SFINAE<U, &U::reserve>*);

    template <typename U>
    static bad reserve(...);

    //static_assert(sizeof(bad) == sizeof(good));
    //static_assert(sizeof(reserve<T>(nullptr)) == sizeof(bad));
    static constexpr bool value = sizeof(reserve<T>(nullptr)) == sizeof(good);
};

class A
{
public:
    size_t reserve;
};

class B{};

int main()
{
    static_assert(has_reserve<A>::value);
    static_assert(!has_reserve<B>::value);
    return 0;
}

