#pragma once

namespace yunuGI
{
    union Color
    {
        float rgba[4]{ 0 };
        struct
        {
            float r, g, b, a;
        };
        // Overloading '==' operator for comparison
        bool operator==(const Color& other) const {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }
        static Color red() { return { 1,0,0,0 }; };
        static Color blue() { return { 0,0,1,0 }; };
        static Color green() { return { 0,1,0,0 }; };
        static Color white() { return { 1,1,1,0 }; };
        static Color black() { return { 0,0,0,0 }; };
    };
}