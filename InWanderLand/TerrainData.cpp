#include "InWanderLand.h"
#include "TerrainData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "SingleNavigationField.h"
#include "DebugMeshes.h"

namespace application
{
    namespace editor
    {
        TerrainData* TerrainData::soleTerrainData{ nullptr };
        Terrain_TemplateData* TerrainData::soleTerrainTemplateData{nullptr};
        DebugStaticMesh* TerrainData::debugMesh{nullptr};
        TerrainData::~TerrainData()
        {
            assert(soleTerrainData == this && "지형 정보는 단 하나만 존재해야 합니다!");
            soleTerrainData = nullptr;
            ClearNodes();
        }
        TerrainData& TerrainData::Instance()
        {
            if (soleTerrainTemplateData == nullptr)
            {
                soleTerrainTemplateData = TemplateDataManager::GetSingletonInstance().GetTemplateData<Terrain_TemplateData>("DefaultTerrainTemplate");
                if (soleTerrainTemplateData == nullptr)
                    soleTerrainTemplateData = TemplateDataManager::GetSingletonInstance().CreateTemplateData<Terrain_TemplateData>("DefaultTerrainTemplate");
            }
            if (soleTerrainData == nullptr)
                soleTerrainData = InstanceManager::GetSingletonInstance().CreateInstance<TerrainData>("DefaultTerrainTemplate");
            return *soleTerrainData;
        }
        TemplateDataManager& TerrainData::templateDataManager = TemplateDataManager::GetSingletonInstance();

        bool TerrainData::EnterDataFromTemplate()
        {
            // 템플릿으로부터 초기화되는 데이터들 처리 영역	

            return true;
        }

        ITemplateData* TerrainData::GetTemplateData()
        {
            return pod.templateData;
        }

        bool TerrainData::SetTemplateData(const std::string& dataName)
        {
            auto ptr = templateDataManager.GetTemplateData(dataName);
            if (ptr == nullptr)
            {
                return false;
            }

            pod.templateData = static_cast<Terrain_TemplateData*>(ptr);

            return true;
        }

        IEditableData* TerrainData::Clone() const
        {
            auto& imanager = InstanceManager::GetSingletonInstance();
            auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

            if (instance != nullptr)
            {
                static_cast<TerrainData*>(instance)->pod = pod;
            }

            return instance;
        }

        palette::PaletteInstance* TerrainData::ApplyAsPaletteInstance()
        {
            return nullptr;
        }

        void TerrainData::ApplyAsPlaytimeObject()
        {
            std::vector<Vector3f> vertexList;
            std::vector<int> indexList;
            MakeUpVerticesList(vertexList, indexList);

#ifdef _DEBUG
            ApplyDebugMesh();
#endif
            SingleNavigationField::Instance().BuildField(vertexList, indexList);
        }
        void TerrainData::MakeUpVerticesList(std::vector<Vector3f>& vertexList, std::vector<int>& indexList)
        {
            for (auto& node : nodes)
            {
                auto nodeKey = node.first;
                bool toprightExists{ nodes.find(nodeKey + Vector2i{1, 1}) != nodes.end() };
                bool topExists{ nodes.find(nodeKey + Vector2i{0, 1}) != nodes.end() };
                bool rightExists{ nodes.find(nodeKey + Vector2i{1, 0}) != nodes.end() };
                if (toprightExists && topExists && rightExists)
                {
                    vertexList.push_back(GetNodePosition(nodeKey));
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 0}));
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{0, 1}));
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 1}));
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 3);
                    indexList.push_back(vertexList.size() - 4);
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 1);
                    indexList.push_back(vertexList.size() - 3);
                }
                else if (toprightExists && topExists)
                {
                    vertexList.push_back(GetNodePosition(nodeKey));
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 1}));
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{0, 1}));
                    indexList.push_back(vertexList.size() - 1);
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 3);
                }
                else if (toprightExists && rightExists)
                {
                    vertexList.push_back(GetNodePosition(nodeKey));
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 1}));
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 0}));
                    indexList.push_back(vertexList.size() - 3);
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 1);
                }
                else if (topExists && rightExists)
                {
                    vertexList.push_back(GetNodePosition(nodeKey));
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{0, 1}));
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 0}));
                    indexList.push_back(vertexList.size() - 3);
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 1);
                }
            }
        }
        void TerrainData::AddNode(const Vector2i& nodeKey, Node&& nodeInfo)
        {
            if (nodes.find(nodeKey) != nodes.end())
                return;
            nodes[nodeKey] = std::move(nodeInfo);
#ifdef _DEBUG
            //nodes[nodeKey].debugObject = CreateNodeDebuggingMesh(nodeKey);
#else
            nodes[nodeKey].debugObject = nullptr;
#endif
        }
        void TerrainData::EraseNode(const Vector2i& nodeKey)
        {
            if (nodes.find(nodeKey) == nodes.end())
                return;
#ifdef _DEBUG
            //Scene::getCurrentScene()->DestroyGameObject(nodes[nodeKey].debugObject);
#endif
            nodes.erase(nodeKey);
        }
        void TerrainData::ClearNodes()
        {
            for (auto& node : nodes)
            {
#ifdef _DEBUG
                //Scene::getCurrentScene()->DestroyGameObject(node.second.debugObject);
#endif
            }
            nodes.clear();
        }
        void TerrainData::ApplyDebugMesh()
        {
            PreSaveCallback();
            std::vector<Vector3f> posVecBefore;
            std::vector<int> idxVecBefore;

            std::vector<yunuGI::Vector3> posVec;
            std::vector<unsigned int> idxVec;
            std::vector<yunuGI::Vector3> normalVec;

            MakeUpVerticesList(posVecBefore, idxVecBefore);
            posVec.resize(posVecBefore.size());
            idxVec.resize(idxVecBefore.size());
            normalVec.resize(posVecBefore.size());
            for (auto i = 0; i < posVecBefore.size(); ++i)
            {
                posVec[i] = { posVecBefore[i].x,posVecBefore[i].y,posVecBefore[i].z };
                normalVec[i] = { 0, 1, 0 };
            };
            for (auto i = 0; i < idxVecBefore.size(); ++i)
                idxVec[i] = idxVecBefore[i];

            if (!posVec.empty() && !idxVec.empty() && !normalVec.empty())
            {
                auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
                rsrcManager->DeleteMesh(GetDebugMesh()->GetGI().GetMesh());
                auto mesh = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->CreateMesh(L"terrainMesh", posVec, idxVec, normalVec);
                GetDebugMesh()->GetGI().SetMesh(mesh);
            }
        }
        Vector3d TerrainData::GetNodePosition(const Vector2i& nodeKey)
        {
            return { nodeKey.x * nodeDistance , 0 ,nodeKey.y * nodeDistance };
        }
        Vector2i TerrainData::WorldToNodeSpace(const Vector3d& worldPos)
        {
            return { (int)(0.5 + worldPos.x / nodeDistance) , (int)(0.5 + worldPos.z / nodeDistance) };
        }
        GameObject* TerrainData::CreateNodeDebuggingMesh(const Vector2i& nodeKey)
        {
            constexpr double nodeHeight = 6;
            auto node = Scene::getCurrentScene()->AddGameObject();
            node->GetTransform()->SetWorldPosition(GetNodePosition(nodeKey) - nodeHeight * Vector3d::up * 0.5);
            node->GetTransform()->SetLocalScale({ nodeDistance, nodeHeight, nodeDistance });
            auto mesh = AttachDebugMesh(node, DebugMeshType::Cube, yunuGI::Color{0.788, 0.647, 0.215}, false);
            mesh->SetIsUpdating(false);
            return node;
        }

        bool TerrainData::PreSaveCallback()
        {
            pod.coordinates.clear();
            pod.heights.clear();
            for (auto each : nodes)
            {
                pod.coordinates.push_back({ each.first.x, each.first.y });
                pod.heights.push_back(each.second.height);
            }
            return true;
        }
        bool TerrainData::PreEncoding(json& data) const
        {
            FieldPreEncoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);
            return true;
        }

        bool TerrainData::PostEncoding(json& data) const
        {
            FieldPostEncoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);
            return true;
        }

        bool TerrainData::PreDecoding(const json& data)
        {
            pod.coordinates.clear();

            FieldPreDecoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);
            return true;
        }

        bool TerrainData::PostDecoding(const json& data)
        {
            FieldPostDecoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);
            for (auto i = 0; i < pod.coordinates.size(); ++i)
            {
                AddNode({ pod.coordinates[i].first,pod.coordinates[i].second }, { pod.heights[i], nullptr });
            }
            ApplyDebugMesh();
            return true;
        }

        TerrainData::TerrainData()
            : pod()
        {
            assert(soleTerrainData == nullptr && "지형 정보는 단 하나만 존재해야 합니다!");
            soleTerrainData = this;
        }

        TerrainData::TerrainData(const std::string& name)
            : IEditableData(), pod()
        {
            assert(soleTerrainData == nullptr && "지형 정보는 단 하나만 존재해야 합니다!");
            soleTerrainData = this;
            pod.templateData = static_cast<Terrain_TemplateData*>(templateDataManager.GetTemplateData(name));
            EnterDataFromTemplate();
        }

        TerrainData::TerrainData(const TerrainData& prototype)
            : pod(prototype.pod)
        {
            assert(soleTerrainData == nullptr && "지형 정보는 단 하나만 존재해야 합니다!");
            soleTerrainData = this;
        }

        TerrainData& TerrainData::operator=(const TerrainData& prototype)
        {
            IEditableData::operator=(prototype);
            pod = prototype.pod;
            return *this;
        }
        DebugStaticMesh* TerrainData::GetDebugMesh()
        {
            if (debugMesh)
                return debugMesh;

            debugMesh = Scene::getCurrentScene()->AddGameObject()->AddComponent<DebugStaticMesh>();
            debugMesh->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::brown(), true));
            return debugMesh;
        }
    }
}
