#include "Interactable_SpikeTrap.h"

#include "InteractableData.h"
#include "DebugMeshes.h"

#include "Unit.h"

#include "SFXManager.h"

#include "YunutyWaitForSeconds.h"
#include "PlayerController.h"

void Interactable_SpikeTrap::Start()
{
    frame = Scene::getCurrentScene()->AddGameObjectFromFBX("SM_SpikeFrame");
    frame->SetParent(GetGameObject());

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
            spike = each;
            break;
        }
    }
}

void Interactable_SpikeTrap::Update()
{
    if (PlayerController::Instance().GetState() == PlayerController::State::Battle)
    {
        spike->SetSelfActive(true);
    }
    else
    {
        spike->SetSelfActive(false);
    }

    static auto eraseList = triggerStay;
    for (auto each : triggerStay)
    {
        if (each->IsAlive())
        {
            eraseList.erase(each);
        }
    }
    for (auto each : eraseList)
    {
        triggerStay.erase(each);
    }

    eraseList.clear();

    if (triggerOn && triggerStay.size() == 0)
    {
        OnInteractableTriggerExit();
    }

    for (auto each : triggerStay)
    {
        if (!interactingList.contains(each))
        {
            OnInteractableTriggerEnter();
            lastCoroutine = StartCoroutine(DoInteraction());
        }
    }
}

void Interactable_SpikeTrap::OnTriggerEnter(physics::Collider* collider)
{
    if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
        PlayerController::Instance().GetState() == PlayerController::State::Battle &&
        colliderUnitComponent != nullptr &&
        colliderUnitComponent->IsPlayerUnit() &&
        colliderUnitComponent->IsAlive())
    {
        triggerStay.insert(colliderUnitComponent);
    }
}

void Interactable_SpikeTrap::OnTriggerExit(physics::Collider* collider)
{
    if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
        colliderUnitComponent != nullptr &&
        colliderUnitComponent->IsPlayerUnit())
    {
        triggerStay.erase(colliderUnitComponent);
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
        SFXManager::PlaySoundfile3D("sounds/trap/Spike damage.wav", each->GetTransform()->GetWorldPosition());
    }

    co_yield yunutyEngine::coroutine::WaitForSeconds(delayTime, false);

    for (auto each : units)
    {
        interactingList.erase(each);
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
