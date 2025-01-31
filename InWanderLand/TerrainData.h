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
                unsigned int debugMeshIndex;
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

            void AddNode(const Vector2i& nodeKey);
            void EraseNode(const Vector2i& nodeKey);
            // 디버그 메시를 새로 만들어 교체,적용한다.
            void ApplyDebugMesh();
            void ClearNodes();
            bool IsEmpty() { return nodes.empty() && pod.coordinates.empty(); }
            Vector3d GetNodePosition(const Vector2i& nodeKey);
            Vector2i WorldToNodeSpace(const Vector3d& worldPos);
            GameObject* CreateNodeDebuggingMesh(const Vector2i& nodeKey);

            POD_Terrain pod;

            static constexpr double nodeDistance = 1;
            // 0 높이에서의 z 파이팅을 방지하기 위해 디버그 메시에 더해주는 높이값
            static constexpr double debugMeshYPos = 0.01;
        protected:
            virtual bool PreSaveCallback()  override;
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            struct DebugMeshInfo
            {
                DebugStaticMesh* debugMesh;
                // 메시생성용 버텍스, 인덱스 리스트
                std::vector<yunuGI::Vector3> vertices;
                std::vector<unsigned int> indices;
                std::vector<yunuGI::Vector3> normals;
                std::unordered_set<Vector2i> nodes;
            };
            static DebugStaticMesh* debugMesh;
            unordered_map<Vector2i, Node> nodes;
            static TerrainData* soleTerrainData;
            static Terrain_TemplateData* soleTerrainTemplateData;
            static TemplateDataManager& templateDataManager;
            bool isTerrainfoCached = false;

            template<typename T>
            void MakeUpVerticesList(std::vector<yunuGI::Vector3>& vertexList, std::vector<unsigned int>& indexList, const T& nodes);
            DebugStaticMesh* MakeDebugMesh();
            unsigned int GetOrCreateDebugMeshIndex();
            static constexpr unsigned int debugMeshVerticeNumThreshold{ 20000 };
            std::vector<DebugMeshInfo> debugMeshes;
            std::set<unsigned int> affectedMeshIndices;
            TerrainData();
            TerrainData(const std::string& name);
            TerrainData(const TerrainData& prototype);
            TerrainData& operator=(const TerrainData& prototype);
        };
    }
}
