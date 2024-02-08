/// 2024. 02. 05 김상준
/// POD 에서 사용할 수 있는 기본적인 Struct 를 제공하는 헤더 파일

#pragma once

#include "Storable.h"
#include <type_traits>

namespace application
{
    namespace editor
    {
        template <typename T> requires std::is_floating_point_v<T>
        struct POD_Vector2
        {
            T x = 0;
            T y = 0;

            TO_JSON(POD_Vector2)
            FROM_JSON(POD_Vector2)
        };

        template <typename T> requires std::is_floating_point_v<T>
        struct POD_Vector3
        {
            T x = 0;
            T y = 0;
            T z = 0;

            TO_JSON(POD_Vector3)
            FROM_JSON(POD_Vector3)
        };

        template <typename T> requires std::is_floating_point_v<T>
        struct POD_Vector4
        {
            T x = 0;
            T y = 0;
            T z = 0;
            T w = 1;

            TO_JSON(POD_Vector4)
            FROM_JSON(POD_Vector4)
        };

        template <typename T> requires std::is_floating_point_v<T>
        struct POD_Quaternion
        {
            T x = 0;
            T y = 0;
            T z = 0;
            T w = 1;

            TO_JSON(POD_Quaternion)
            FROM_JSON(POD_Quaternion)
        };
    }
}
