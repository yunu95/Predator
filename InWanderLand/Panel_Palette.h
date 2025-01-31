/// 2023. 11. 01 김상준
/// UnitData, TerrainData 등 대단위에 대한 편집 시 옵션을 제공하는 팔레트 패널

#pragma once

#include "Singleton.h"
#include "EditorPanel.h"
#include "EditorResourceManager.h"
#include "EditorInputManager.h"
#include "TemplateDataManager.h"
#include "PaletteManager.h"
#include "PaletteList.h"
#include "EditorCamera.h"

#include <vector>

namespace yunutyEngine
{
    class GameObject;
}

namespace application
{
    namespace editor
    {
        class LightData;
    }
}

namespace application
{
    namespace editor
    {
        class PalettePanel
            : public Panel, public Singleton<PalettePanel>
        {
            friend class Singleton<PalettePanel>;

        public:
            virtual ~PalettePanel();

            virtual void Initialize() override;
            virtual void Update(float ts) override;
            virtual void GUIProgress() override;
            virtual void Finalize() override;
            virtual void OnEvent(EditorEvents& event) override;

            virtual void OnPlayContents() override;
            virtual void OnPauseContents() override;
            virtual void OnResumeContents() override;
            virtual void OnStopContents() override;
 
            void ChangeTab(const std::string& tabName);
            void ChangeDirectionalLight(LightData* light);

        private:
            PalettePanel();

            void ChangePalette(palette::Palette* palette);

            void ImGui_Update();

            void ImGui_BeginTerrainPalette();
            void ImGui_BeginUnitPalette();
            void ImGui_BeginOrnamentPalette();
            void ImGui_BeginRegionPalette();
            void ImGui_BeginWavePalette();
            void ImGui_BeginLightPalette();
            void ImGui_BeginCameraPalette();
            void ImGui_BeginParticlePalette();
            void ImGui_BeginInteractablePalette();

            bool ImGui_CreateUnitPopup();
            bool ImGui_CreateParticlePopup();
            bool ImGui_CreateInteractablePopup();

            void LoadCallback();
            void UpdataLightGizmo();

            palette::Palette* currentPalette = nullptr;

            ResourceManager& erm = ResourceManager::GetSingletonInstance();
            EditorInputManager& eim = EditorInputManager::GetSingletonInstance();
            TemplateDataManager& tdm = TemplateDataManager::GetSingletonInstance();
            palette::PaletteManager& pm = palette::PaletteManager::GetSingletonInstance();
            palette::TerrainPalette& tp = palette::TerrainPalette::SingleInstance();
            palette::UnitPalette& up = palette::UnitPalette::SingleInstance();
            palette::OrnamentPalette& op = palette::OrnamentPalette::SingleInstance();
            palette::RegionPalette& rp = palette::RegionPalette::SingleInstance();
            palette::WavePalette& wp = palette::WavePalette::SingleInstance();
            palette::CameraPalette& cp = palette::CameraPalette::SingleInstance();
            palette::LightPalette& lp = palette::LightPalette::SingleInstance();
            palette::ParticlePalette& pp = palette::ParticlePalette::SingleInstance();
            palette::InteractablePalette& ip = palette::InteractablePalette::SingleInstance();

            EditorCamera& ec = EditorCamera::GetSingletonInstance();
            LightData* directionalLight = nullptr;
            GameObject* lightGizmo = nullptr;

            int unitCurrentButton = -1;
            int ornamentCurrentButton = -1;
            int lightCurrentButton = -1;
            int interactableCurrentButton = -1;
        };
    }
}
