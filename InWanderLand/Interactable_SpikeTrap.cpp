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
    if (PlayerController::Instance().GetState() == PlayerController::State::Battle || PlayerController::Instance().GetState() == PlayerController::State::Tactic)
    {
        if (!spike->GetActive())
        {
            appearAndHideCoro = StartCoroutine(AppearCoroutine());
        }
    }
    else
    {
        if (spike->GetActive() && appearAndHideCoro.expired())
        {
            appearAndHideCoro = StartCoroutine(HideCoroutine());
        }
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
        (PlayerController::Instance().GetState() == PlayerController::State::Battle || PlayerController::Instance().GetState() == PlayerController::State::Tactic) &&
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

void Interactable_SpikeTrap::Recovery()
{
    if (!lastCoroutine.expired())
    {
        DeleteCoroutine(lastCoroutine);
        interactingList.clear();
    }

    if (!appearAndHideCoro.expired())
    {
        DeleteCoroutine(appearAndHideCoro);
        spike->SetSelfActive(false);
    }
}

yunutyEngine::coroutine::Coroutine Interactable_SpikeTrap::AppearCoroutine()
{
    spike->SetSelfActive(true);
    spike->GetTransform()->SetLocalPosition(Vector3d(0, -offset_y, 0));

    float coroSpeed = offset_y * 2 / bounceTime;

    yunutyEngine::coroutine::ForSeconds wait = yunutyEngine::coroutine::ForSeconds(1 / coroSpeed);
    yunutyEngine::coroutine::ForSeconds wait2 = yunutyEngine::coroutine::ForSeconds(bounceTime);

    while (wait.Tick())
    {
        spike->GetTransform()->SetLocalPosition(Vector3d(0, math::LerpF(-offset_y, 0, wait.ElapsedNormalized()), 0));
        co_await std::suspend_always{};
    }

    while (wait2.Tick())
    {
        spike->GetTransform()->SetLocalScale(Vector3d(1, 1, 1 + 0.1 * std::sin(wait2.ElapsedNormalized() * math::PI)));
        co_await std::suspend_always{};
    }

    spike->GetTransform()->SetLocalScale(Vector3d(1, 1, 1));
    co_return;
}

yunutyEngine::coroutine::Coroutine Interactable_SpikeTrap::HideCoroutine()
{
    spike->GetTransform()->SetLocalScale(Vector3d(1, 1, 1));

    yunutyEngine::coroutine::ForSeconds wait = yunutyEngine::coroutine::ForSeconds(bounceTime);

    while (wait.Tick())
    {

        spike->GetTransform()->SetLocalPosition(Vector3d(0, math::LerpF(0, -offset_y, wait.ElapsedNormalized()), 0));
        co_await std::suspend_always{};
    }

    spike->SetSelfActive(false);
    co_return;
}
