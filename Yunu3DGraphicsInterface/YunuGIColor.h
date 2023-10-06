#pragma once

namespace yunuGI
{
    union Color
    {
        float rgba[4]{0};
        struct 
        {
            float r, g, b, a;
        };
        static Color red() { return { 1,0,0,0 }; };
        static Color blue() { return { 0,0,1,0 }; };
        static Color green() { return { 0,1,0,0 }; };
    };
}