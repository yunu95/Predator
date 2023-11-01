#pragma once
#include "YunutyEngine.h"
#include "PathField.h"
using namespace yunutyEngine;

// RTS 경로를 마우스 클릭으로 생성할 수 있게 만들기 위한 에디터 기능
// 어떤 영역에 변경사항이 적용될지 그래픽 이미지를 표시한다.
class PathEditor : public Component
{
public:
    // 수정의 대상이 되는 경로필드
    PathField* targetPathField{ nullptr };
    PathEditor()
    {
        nodeHoverDebugMesh = PathField::CreateNodeDebugMesh();
        nodeHoverDebugMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());
        nodeHoverDebugMesh->GetGI().GetMaterial()->SetPixelShader(L"DebugPS.cso");
        nodeHoverDebugMesh->GetGameObject()->SetSelfActive(false);
    }
    // 경로 위에 마우스를 호버하고 있을때 변경사항이 적용될 영역을 표시하기 위한 호버링 함수
    void PositionHover(const Vector3d& position)
    {
        if (auto node = targetPathField->GetNode(position))
        {
            auto nodePosition = targetPathField->GetNearestNodePosition(position);
            nodeHoverDebugMesh->GetGameObject()->SetSelfActive(true);
            nodeHoverDebugMesh->GetTransform()->SetWorldPosition(nodePosition);
            nodeHoverDebugMesh->Update();
        }
    }
    // 왼클릭이 일어났을 때 변경사항을 적용하기 위해 쓰이는 함수. RTSCam의 좌클릭 콜백함수로 쓰인다.
    void OnLeftClick(const Vector3d& position)
    {
        if (auto node = targetPathField->GetNode(position))
        {
            auto nodePosition = targetPathField->GetNearestNodePosition(position);
            if (!node->passable)
            {
                node->debugMesh = PathField::CreateNodeDebugMesh();
                node->debugMesh->GetTransform()->SetWorldPosition(nodePosition);
                node->debugMesh->Update();
                node->passable = true;
            }
            else
            {
                Scene::getCurrentScene()->DestroyGameObject(node->debugMesh->GetGameObject());
                node->passable = false;
            }
        }
    }
private:
    // 호버링 된 노드에 표시할 디버그 메시
    graphics::StaticMeshRenderer* nodeHoverDebugMesh{};
};
