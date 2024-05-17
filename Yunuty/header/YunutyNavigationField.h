#pragma once
#include "YunutyEngine.h"
#include "Component.h"
#include "Vector3.h"
#include <vector>
#include <assert.h>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    class NavigationAgent;
    class NavigationObstacle;
    class YUNUTY_API NavigationField : public Component
    {
    public:
        class Impl;
        struct BuildSettings
        {
            static constexpr float tilingScale = 4.0f;
            // 길찾기 주체들의 최대 개체수
            int maxCrowdNumber{ 1024 };
            // 길찾기 주체들의 최대 충돌반경
            float maxAgentRadius{ 10.0 };
            // 오를수 있는 경사
            float walkableSlopeAngle{ 80 };
            // 오를 수 있는 단차
            float walkableClimb{ 0.2 };
            // 천장의 최소 높이
            float walkableHeight{ 0.3 };
            // x축,z축 공간 분할의 단위, 단위가 작을수록 판정이 더 세밀해지지만, 네비게이션 빌드와 길찾기 시스템의 부하가 늘게 된다.
            float divisionSizeXZ{ 0.3 * tilingScale };
            // y축 공간 분할의 단위, 단위가 작을수록 판정이 더 세밀해지지만, 네비게이션 빌드와 길찾기 시스템의 부하가 늘게 된다.
            float divisionSizeY{ 0.2 * tilingScale };
            // 공간 분할은 xz축으로 250*330, y축으로 200개 정도 분할되는 정도면 순식간에 네비게이션 빌드도 되면서 길찾기도 무리없이 하게 되는 정도다.
            // xz축으로 743* 989개 정도 분할이 되도 큰 부하는 없다.
            float tileSize{ 48 };
            float agentRadius{ 0.6 };
            float edgeMaxError{ 1.3 };
        };
        NavigationField();
        virtual ~NavigationField();
        virtual void Update();
        Vector3d GetClosestPointOnField(const Vector3d& worldPosition) const;
        std::vector<Vector3d> GetSmoothPath(const Vector3d& start, const Vector3d& end);
        void BuildField(const float* worldVertices, size_t verticesNum, const int* faces, size_t facesNum, const NavigationField::BuildSettings& buildSettings = BuildSettings{});
        bool IsInitialized();
        // faces는 3개의 정수로 이루어진 삼각형 인덱스 배열이다.
        // 삼각형의 노말 방향은 왼손 법칙을 따르므로, 0-1, 1-2 버텍스를 잇는 화살표는 시계방향으로 감겨야 한다.
        void BuildField(std::vector<Vector3f> worldVertices, std::vector<int> faces, const NavigationField::BuildSettings& buildSettings = BuildSettings{})
        {
            static_assert(sizeof(Vector3f) == sizeof(float) * 3);
            assert(!worldVertices.empty() && !faces.empty());
            assert(faces.size() % 3 == 0);
            BuildField(reinterpret_cast<float*>(&worldVertices[0]), worldVertices.size(), &faces[0], faces.size() / 3, buildSettings);
        }
        //bool AddCylinderObstacle(Vector3f position, const float radius, const float height);
        //bool AddBoxObstacle(Vector3f center, Vector3f halfExtents, const float yRadians);
    private:
        unordered_set<NavigationAgent*> agents;
        unordered_set<NavigationObstacle*> obstacles;
        // CleanUp시 NavigationAgent와 Obstacles들에 적절한 조치 취하게 만들기.
        void CleanUpField();
        Impl* impl{ nullptr };
        friend NavigationAgent;
        friend NavigationObstacle;
    };
}
