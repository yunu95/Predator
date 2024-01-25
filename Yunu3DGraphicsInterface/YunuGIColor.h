#pragma once
#include <type_traits>

namespace yunuGI
{
    struct Color
    {
        float r{ 1 };
        float g{ 1 };
        float b{ 1 };
        float a{ 1 };
        // Overloading '==' operator for comparison
        bool operator==(const Color& other) const {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }
        static Color red() { return { 1,0,0,1 }; };
        static Color blue() { return { 0,0,1,1 }; };
        static Color green() { return { 0,1,0,1 }; };
        static Color white() { return { 1,1,1,1 }; };
        static Color black() { return { 0,0,0,1 }; };
        static Color gray() { return { 0.9,0.9,0.9,1 }; };
    };
}
template <>
struct std::hash<yunuGI::Color>
{
    std::size_t operator()(const yunuGI::Color& k) const
    {
        using std::size_t;
        using std::hash;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        return(hash<float>()(k.r) << 0)
            ^ (hash<float>()(k.g) << 1)
            ^ (hash<float>()(k.b) << 2)
            ^ (hash<float>()(k.a) << 3);
    }
};