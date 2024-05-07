#include "ParticlePalette.h"
#include "WavePalette.h"
#include "ParticleEditorInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Particle_TemplateData.h"
#include "ParticleData.h"
#include "TemplateDataManager.h"
#include "ParticleBrush.h"

namespace application::editor::palette
{
    ParticleData* ParticlePalette::GetSingleSelectedParticle()
    {
        return selection.empty() ? nullptr : static_cast<ParticleData*>(const_cast<IEditableData*>(*selection.begin()));
    }

    void ParticlePalette::SelectParticle(ParticleData* particle)
    {
        Palette::OnSelectSingleInstance(particle);
    }

    void ParticlePalette::SelectParticleTemplateData(Particle_TemplateData* templateData)
    {
        selectedParticleTemplateData = templateData;
        if (selectedParticleTemplateData == nullptr)
        {
            ParticleBrush::Instance().ReadyBrush("");
        }
        else
        {
            ParticleBrush::Instance().ReadyBrush(selectedParticleTemplateData->GetDataKey());
        }
    }

    void ParticlePalette::UnselectParticleTemplateData()
    {
        SelectParticleTemplateData(nullptr);
    }

    void ParticlePalette::Initialize()
    {
        TemplateDataManager::GetSingletonInstance().CreateTemplateData("DefaultParticle", DataType::ParticleData);
    }

    void ParticlePalette::Reset()
    {
        Palette::Reset();
        UnselectParticleTemplateData();
    }

    IEditableData* ParticlePalette::PlaceInstance(Vector3d worldPosition)
    {
        if (selectedParticleTemplateData == nullptr)
            return nullptr;

        auto instance = InstanceManager::GetSingletonInstance().CreateInstance<ParticleData>("DefaultParticle");
        instance->pod.position.x = worldPosition.x;
        instance->pod.position.y = worldPosition.y;
        instance->pod.position.z = worldPosition.z;

        instance->ApplyAsPaletteInstance();
        return instance;
    }

    void ParticlePalette::OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos)
    {
        Palette::OnMouseMove(projectedWorldPos, normalizedScreenPos);
        // 브러시 움직이기
        ParticleBrush::Instance().GetTransform()->SetWorldPosition(projectedWorldPos);
        if (IsClickingLeft() && !IsSelectMode() && CheckInstantiationCooltime())
            PlaceInstance(projectedWorldPos);
    }

    void ParticlePalette::SetAsSelectMode(bool isSelectMode)
    {
        Palette::SetAsSelectMode(isSelectMode);
        if (isSelectMode)
        {
            ParticleBrush::Instance().ReadyBrush("");
        }
        else
        {
            SelectParticleTemplateData(selectedParticleTemplateData);
        }
    }

    bool ParticlePalette::ShouldSelect(IEditableData* instance)
    {
        return dynamic_cast<ParticleData*>(instance);
    }

    void ParticlePalette::OnStartPalette()
    {
        switch (beforeState)
        {
            case application::editor::palette::Palette::State::Place:
            {
                SetAsSelectMode(false);
                break;
            }
            default:
                SetAsSelectMode(true);
                break;
        }
        ParticleBrush::Instance().GetGameObject()->SetSelfActive(true);
    }

    void ParticlePalette::OnStandbyPalette()
    {
        if (IsSelectMode())
        {
            beforeState = State::Select;
        }
        else
        {
            beforeState = State::Place;
        }
        state = State::None;
        ParticleBrush::Instance().GetGameObject()->SetSelfActive(false);
        CleanUpData();
    }
}
