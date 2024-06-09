#include "Interactable_SpikeTrap.h"

#include "InteractableData.h"
#include "DebugMeshes.h"

#include "Unit.h"
#include "GameManager.h"
#include "SFXManager.h"

#include "YunutyWaitForSeconds.h"

void Interactable_SpikeTrap::Start()
{
    auto ts = GetGameObject()->GetTransform();
    ts->SetWorldPosition(initPos);
    ts->SetWorldRotation(initRotation);
    ts->SetWorldScale(initScale);

    auto rendererObj = GetGameObject()->AddGameObject();
    AttachDebugMesh(rendererObj, DebugMeshType::Cube, yunuGI::Color::green());
    rendererObj->GetTransform()->SetLocalScale(Vector3d(2.5, 2.5, 2.5));
    auto boxCollider = GetGameObject()->AddComponent<physics::BoxCollider>();
    boxCollider->SetHalfExtent(Vector3d(1.25, 1.25, 1.25));

    for (auto each : GetGameObject()->GetChildren())
    {
        auto renderer = each->GetComponent<graphics::StaticMeshRenderer>();
        if (renderer)
        {
            mesh = each;
            break;
        }
    }
}

void Interactable_SpikeTrap::Update()
{
    for (auto each : triggerStay)
    {
        if (!interactingList.contains(each))
        {
            OnInteractableTriggerEnter();
            lastCoroutine = StartCoroutine(DoInteraction());
        }
    }

    /// 내부로 들어온 유닛이 죽었을 때, 리스트에서 제외하는 로직 필요함
}

void Interactable_SpikeTrap::OnTriggerEnter(physics::Collider* collider)
{
    if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
        //GameManager::Instance().IsBattleSystemOperating() &&
        colliderUnitComponent != nullptr &&
        colliderUnitComponent->IsPlayerUnit() &&
        colliderUnitComponent->IsAlive())
    {
        triggerStay.insert(collider);
    }
}

void Interactable_SpikeTrap::OnTriggerExit(physics::Collider* collider)
{
    if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
        //GameManager::Instance().IsBattleSystemOperating() &&
        colliderUnitComponent != nullptr &&
        colliderUnitComponent->IsPlayerUnit())
    {
        if (triggerStay.size() == 1 && triggerOn)
        {
            OnInteractableTriggerExit();
        }
        triggerStay.erase(collider);
    }
}

yunutyEngine::coroutine::Coroutine Interactable_SpikeTrap::DoInteraction()
{
    std::unordered_set<Unit*> units = std::unordered_set<Unit*>();
    for (auto each : triggerStay)
    {
        if (!interactingList.contains(each))
        {
            interactingList.insert(each);
            units.insert(each->GetGameObject()->GetComponent<Unit>());
        }
    }

    for (auto each : units)
    {
        each->KnockBackRelativeVector(5 * (each->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition()).Normalized(), 1);
        each->Damaged(damage);
        SFXManager::PlaySoundfile3D("sounds/trap/Damage_Zone.wav", each->GetTransform()->GetWorldPosition());
    }

    co_yield yunutyEngine::coroutine::WaitForSeconds(delayTime, false);

    for (auto each : units)
    {
        interactingList.erase(each->GetGameObject()->GetComponent<physics::Collider>());
    }

    co_return;
}

void Interactable_SpikeTrap::SetDataFromEditorData(const application::editor::InteractableData& data)
{
    initPos.x = data.pod.position.x;
    initPos.y = data.pod.position.y;
    initPos.z = data.pod.position.z;
    initRotation.w = data.pod.rotation.w;
    initRotation.x = data.pod.rotation.x;
    initRotation.y = data.pod.rotation.y;
    initRotation.z = data.pod.rotation.z;
    initScale.x = data.pod.scale.x;
    initScale.y = data.pod.scale.y;
    initScale.z = data.pod.scale.z;
    delayTime = data.pod.templateData->pod.delayTime;
    damage = data.pod.templateData->pod.damage;
}
