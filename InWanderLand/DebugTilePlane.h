#pragma once

#include "YunutyEngine.h"

using namespace yunutyEngine;
// 임시로 타일 평면을 사방으로 배치할 수 있는 모듈
class DebugTilePlane : public yunutyEngine::Component
{
public:
    int width{ 5 };
    int height{ 5 };
    yunuGI::Color colorA{1, 0.5, 0.25, 1};
    yunuGI::Color colorB{0.5, 0.25, 0.25, 1};
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
            /*for (int j = 0; j < width; j++)
            {
                Vector3d position = GetTransform()->GetWorldPosition() + Vector3d(j - halveWidth+0.5, -0.5,i - halveHeight+0.5);
                auto tile = Scene::getCurrentScene()->AddGameObject();
                auto staticMesh = tile->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
                staticMesh->GetGI().LoadMesh("Cube");
                staticMesh->GetGI().GetMaterial()->SetColor(((i + j) % 2 == 0 ? colorA : colorB));
                tile->GetTransform()->SetWorldPosition(position);
                staticMesh->GetTransform()->scale = Vector3d(1, 1, 1);
            }*/
            Vector3d position = GetTransform()->GetWorldPosition() + Vector3d(0, -0.5, i - halveHeight + 0.5);

            auto tile = Scene::getCurrentScene()->AddGameObject();
            auto staticMesh = tile->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();

            const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
            auto& meshList = _resourceManager->GetMeshList();
            for (auto& e : meshList)
            {
                if (e->GetName() == L"Cube")
                {
                    staticMesh->GetGI().SetMesh(e);
                }
            }

            staticMesh->GetGI().GetMaterial()->SetColor(((i) % 2 == 0 ? colorA : colorB));
            tile->GetTransform()->SetWorldPosition(position);
            staticMesh->GetTransform()->scale = Vector3d(static_cast<double>(width), 1, 1);
        }
    }
};
