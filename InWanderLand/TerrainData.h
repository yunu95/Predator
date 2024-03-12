/// 2023. 11. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 지형

#pragma once

#include "IEditableData.h"
#include "Terrain_TemplateData.h"
#include "DebugStaticMesh.h"
#include "GlobalConstant.h"

#include <memory>
#include <string>

namespace application
{
    namespace editor
    {
        class TemplateDataManager;
    }
}

namespace application
{
    namespace editor
    {
        class TerrainData;
        struct POD_Terrain
        {
            Terrain_TemplateData* templateData = nullptr;
            std::vector<std::pair<int, int>> coordinates;
            std::vector<float> heights;

            TO_JSON(POD_Terrain)
                FROM_JSON(POD_Terrain)
        };

        class TerrainData
            : public IEditableData
        {
            friend class InstanceManager;

        public:
            struct Node
            {
                float height;
                GameObject* debugObject;
            };
            ~TerrainData();
            // 지형 인스턴스 정보는 맵에 단 하나만 존재합니다.
            static TerrainData& Instance();
            virtual bool EnterDataFromTemplate() override;
            virtual ITemplateData* GetTemplateData() override;
            virtual bool SetTemplateData(const std::string& dataName) override;
            virtual IEditableData* Clone() const override;
            virtual palette::PaletteInstance* ApplyAsPaletteInstance()override;
            virtual void ApplyAsPlaytimeObject()override;
            virtual bool EnterDataFromGlobalConstant() override { return true; };

            void MakeUpVerticesList(std::vector<Vector3f>& vertexList, std::vector<int>& indexList);

                void AddNode(const Vector2i& nodeKey, Node&& nodeInfo);
            void EraseNode(const Vector2i& nodeKey);
            // 디버그 메시를 새로 만들어 교체,적용한다.
            void ApplyDebugMesh();
            void ClearNodes();
            Vector3d GetNodePosition(const Vector2i& nodeKey);
            Vector2i WorldToNodeSpace(const Vector3d& worldPos);
            GameObject* CreateNodeDebuggingMesh(const Vector2i& nodeKey);

            POD_Terrain pod;

            static constexpr double nodeDistance = 1;
        protected:
            virtual bool PreSaveCallback()  override;
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static DebugStaticMesh* debugMesh;
            unordered_map<Vector2i, Node> nodes;
            static TerrainData* soleTerrainData;
            static Terrain_TemplateData* soleTerrainTemplateData;
            static TemplateDataManager& templateDataManager;

            static DebugStaticMesh* GetDebugMesh();
            TerrainData();
            TerrainData(const std::string& name);
            TerrainData(const TerrainData& prototype);
            TerrainData& operator=(const TerrainData& prototype);
        };
    }
}
