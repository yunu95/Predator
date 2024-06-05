#pragma once
#include <random>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;

namespace yunutyEngine
{
    namespace math
    {
        class YUNUTY_API Random
        {
        public:
            static int GetRandomInt(int min, int max);
            static float GetRandomFloat(float min, float max);
            static float GetRandomFloat(float amplitude);
        private:
            Random();
            static Random* SingleInstance();
            static Random* instance;
            random_device rd;
            mt19937 gen;
        };
    }
}
