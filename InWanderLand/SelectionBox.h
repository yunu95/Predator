#pragma once
#include "PaletteManager.h"
#include "PaletteInstance.h"
#include "Palette.h"
#include "SelectionBox.h"
#include "YunutyEngine.h"
#include "SingletonComponent.h"
#include "DebugMeshes.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class SelectionBox : public yunutyEngine::Component, public yunutyEngine::SingletonComponent<SelectionBox>
            {
            public:
                virtual void Start() override
                {
                    auto rsrcManager = graphics::Renderer::SingleInstance().GetResourceManager();
                    boxMesh = AttachDebugMesh(GetGameObject(), DebugMeshType::Rectangle, yunuGI::Color::green(), true);
                    boxMesh->GetGI().SetMaterial(0, GetColoredDebugMaterialTransparent({ 0,1,0,0.2 }));
                    boxCollider = GetGameObject()->AddComponent<yunutyEngine::physics::BoxCollider>();
                    //debugBoxColliderNear = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::physics::BoxCollider>();
                    //debugBoxColliderMiddle = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::physics::BoxCollider>();
                    //debugBoxColliderFar = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::physics::BoxCollider>();
                    ShowSelectionBox(false);
                }
                void SetCoverage(Vector3d pointA, Vector3d pointB)
                {
                    ShowSelectionBox(true);
                    auto transform = GetTransform();
                    transform->SetLocalRotation(Quaternion({ 90,0,0 }));
                    transform->SetWorldPosition(((pointA + pointB) / 2));
                    transform->SetLocalScale({ (pointA - pointB).Abs() });
                    auto newScale = transform->GetLocalScale();
                    newScale.y = newScale.z;
                    transform->SetLocalScale(newScale);
                }
                // 카메라의 near plane으로부터 far plane까지 뻗는 직선의 형태로
                // 셀렉션 박스의 트랜스폼을 변경합니다.
                // screenSpacePos는 x,y좌표가 -0.5에서 0.5 사이의 값을 가지는 정규화된 스크린좌표입니다.
                // 화면의 중심이 {0,0} 상단이 0.5, 하단이 -0.5인 좌단이 -0.5, 우단이 0.5입니다.
                void SetAsPickingCollider(graphics::Camera* cam, Vector2d screenSpacePos)
                {
                    ShowSelectionBox(false);
                    //auto nearPoint = cam->GetProjectedPoint(screenSpacePos, 0);
                    auto nearPoint = cam->GetTransform()->GetWorldPosition() - cam->GetTransform()->GetWorldRotation().Forward() * cam->GetGI().GetNear();
                    auto farPoint = cam->GetProjectedPoint(screenSpacePos, 100);
                    //auto farPoint = cam->GetProjectedPoint(screenSpacePos, cam->GetGI().GetFar());

                    //debugBoxColliderNear->GetTransform()->SetLocalPosition(nearPoint);
                    //debugBoxColliderMiddle->GetTransform()->SetLocalPosition((nearPoint + farPoint) / 2.0);
                    //debugBoxColliderFar->GetTransform()->SetLocalPosition(farPoint);

                    GetTransform()->SetLocalScale({ 0.1,0.1, (nearPoint - farPoint).Magnitude() });
                    GetTransform()->SetWorldPosition((nearPoint + farPoint) / 2.0);
                    GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(farPoint - nearPoint, cam->GetTransform()->GetWorldRotation().Up()));
                }
                void ShowSelectionBox(bool box)
                {
                    if (boxMesh)
                        boxMesh->SetActive(box);
                }
                unordered_set<PaletteInstance*>& GetContactingInstances()
                {
                    return contactingInstances;
                }
            protected:
                virtual void OnTriggerEnter(physics::Collider* other) override
                {
                    if (auto parentObj = other->GetGameObject()->GetParentGameObject(); parentObj != nullptr)
                    {
                        if (auto instance = parentObj->GetComponent<PaletteInstance>())
                        {
                            contactingInstances.insert(instance);
                            if (pm.GetCurrentPalette()->ShouldSelect(instance->GetEditableData()))
                                pm.GetCurrentPalette()->OnSelectionContactEnter(instance->GetEditableData());
                        }
                    }
                }
                virtual void OnTriggerExit(physics::Collider* other) override
                {
                    if (other == nullptr)
                        return;
                    if (auto parentObj = other->GetGameObject()->GetParentGameObject(); parentObj != nullptr)
                    {
                        if (auto instance = parentObj->GetComponent<PaletteInstance>(); instance != nullptr)
                        {
                            contactingInstances.erase(instance);
                            if (pm.GetCurrentPalette()->ShouldSelect(instance->GetEditableData()))
                                pm.GetCurrentPalette()->OnSelectionContactExit(instance->GetEditableData());
                        }
                    }
                }
            private:
                yunutyEngine::physics::BoxCollider* boxCollider;
                //yunutyEngine::physics::BoxCollider* debugBoxColliderNear;
                //yunutyEngine::physics::BoxCollider* debugBoxColliderMiddle;
                //yunutyEngine::physics::BoxCollider* debugBoxColliderFar;
                PaletteManager& pm = PaletteManager::GetSingletonInstance();
                unordered_set<PaletteInstance*> contactingInstances;
                graphics::StaticMeshRenderer* boxMesh;
            };
        }
    }
}
