#pragma once

#include "YunutyEngine.h"

using namespace yunutyEngine;
// 임시로 타일 평면을 사방으로 배치할 수 있는 모듈
class DebugTilePlane : public yunutyEngine::Component
{
public:
    int width{ 5 };
    int height{ 5 };
    yunuGI::Color colorA{1, 0.5, 0.25, 0};
    yunuGI::Color colorB{0.5, 0.25, 0.25, 0};
    vector<GameObject*> tiles;
    void SetTiles()
    {
        for (auto each : tiles)
            Scene::getCurrentScene()->DestroyGameObject(each);

        tiles.clear();
        double halveWidth = width / 2.0f;
        double halveHeight = height / 2.0f;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                Vector3d position = GetTransform()->GetWorldPosition() + Vector3d(i - halveWidth, j - halveHeight, 0.5);
                auto tile = Scene::getCurrentScene()->AddGameObject();
                auto staticMesh = tile->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
                staticMesh->GetGI().LoadMesh("Cube");
                //staticMesh->GetGI().SetMesh(L"Cube");
                //staticMesh->GetGI().SetColor(0, ((i + j) % 2 == 0 ? colorA : colorB));
                //staticMesh->GetGI().SetShader(0, L"Forward");
                //staticMesh->GetGI().SetMaterialName(0, L"Forward");
                tile->GetTransform()->SetWorldPosition(position);
                staticMesh->GetTransform()->scale = Vector3d(1, 1, 1);
            }
        }
    }
};
