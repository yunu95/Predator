#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"
#include "DebugMeshes.h" 
#include "TerrainPalette.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class TerrainBrush : public yunutyEngine::Component,public yunutyEngine::SingletonComponent<TerrainBrush>
            {
            public:
                void SetBrushSize(unsigned int size)
                {
                    auto expectedNum = (size * 2 + 1) * (size * 2 + 1);

                    for (int i = planeMeshes.size(); i < expectedNum; i++)
                    {
                        auto planeMesh = AttachDebugMesh(GetGameObject()->AddGameObject(), DebugMeshType::Rectangle, yunuGI::Color::green(), true);
                        planeMesh->GetTransform()->scale = Vector3d::one * TerrainPalette::nodeDistance;
                        planeMeshes.push_back(planeMesh);
                        /*
                        25 26 27 28 29 30 31
                        48 09 10 11 12 13 32
                        47 24 01 02 03 14 33
                        46 23 08 00 04 15 34
                        45 22 07 06 05 16 35
                        44 21 20 19 18 17 36
                        43 42 41 40 39 38 37
                        */
                        // 레이어 번호 1    2    3    4
                        // 루트 값    1,2  3,4  5,6  7,8
                        int layerIdx = sqrt(i) / 2.0 + 0.5;

                        // 우방 기수번호 = (레이어 *2+1)^2
                        // 하방 기수번호 = (레이어 *2+1)^2 + 레이어 * 2 * 1
                        // 좌방 기수번호 = (레이어 *2+1)^2 + 레이어 * 2 * 2
                        // 상방 기수번호 = (레이어 *2+1)^2 + 레이어 * 2 * 3

                        // 우방 기수좌표 = -레이어 * TerrainPalette::nodeDistance, 0, +레이어 * TerrainPalette::nodeDistance
                        // 하방 기수번호 = +레이어 * TerrainPalette::nodeDistance, 0, +레이어 * TerrainPalette::nodeDistance
                        // 좌방 기수번호 = +레이어 * TerrainPalette::nodeDistance, 0, -레이어 * TerrainPalette::nodeDistance
                        // 상방 기수번호 = -레이어 * TerrainPalette::nodeDistance, 0, -레이어 * TerrainPalette::nodeDistance

                        // 우방~상방 = 0~3
                        double sx[4] = {
                            -layerIdx * TerrainPalette::nodeDistance,
                            +layerIdx * TerrainPalette::nodeDistance,
                            +layerIdx * TerrainPalette::nodeDistance,
                            -layerIdx * TerrainPalette::nodeDistance,
                        };
                        double sy[4] = {
                            +layerIdx * TerrainPalette::nodeDistance,
                            +layerIdx * TerrainPalette::nodeDistance,
                            -layerIdx * TerrainPalette::nodeDistance,
                            -layerIdx * TerrainPalette::nodeDistance,
                        };
                        constexpr double dx[4] = { TerrainPalette::nodeDistance, 0, -TerrainPalette::nodeDistance, 0 };
                        constexpr double dy[4] = { 0, -TerrainPalette::nodeDistance, 0, TerrainPalette::nodeDistance };
                        unsigned int lineType = layerIdx == 0 ? 0 : (i - (layerIdx * 2 - 1) * (layerIdx * 2 - 1)) / (layerIdx * 2);
                        double deltaIdx = layerIdx == 0 ? 0 : i - ((layerIdx * 2 - 1) * (layerIdx * 2 - 1) + layerIdx * 2 * lineType);

                        planeMesh->GetTransform()->position = { sx[lineType] + deltaIdx * dx[lineType], 0, sy[lineType] + deltaIdx * dy[lineType] };
                        planeMesh->GetTransform()->rotation = { 90,0,0 };
                        planeMeshesPos.push_back(planeMesh->GetTransform()->position);
                    }
                    for (auto i = 0; i < planeMeshes.size(); i++)
                    {
                        if (i < expectedNum)
                            planeMeshes[i]->SetActive(true);
                        else
                            planeMeshes[i]->SetActive(false);
                    }
                }
            protected:
            private:
                /*
                25 26 27 28 29 30 31
                48 09 10 11 12 13 32
                47 24 01 02 03 14 33
                46 23 08 00 04 15 34
                45 22 07 06 05 16 35
                44 21 20 19 18 17 36
                43 42 41 40 39 38 37
                */
                vector<graphics::StaticMeshRenderer*> planeMeshes;
                vector<Vector3d> planeMeshesPos;
            };
        }
    }
}
