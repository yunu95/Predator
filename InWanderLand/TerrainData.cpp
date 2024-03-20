#include <type_traits>
#include "InWanderLand.h"
#include "TerrainData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "SingleNavigationField.h"
#include "DebugMeshes.h"
#include "HasVariable.h"

namespace application
{
    namespace editor
    {
        TerrainData* TerrainData::soleTerrainData{ nullptr };
        Terrain_TemplateData* TerrainData::soleTerrainTemplateData{ nullptr };
        DebugStaticMesh* TerrainData::debugMesh{ nullptr };
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
            if (!nodes.empty())
                PreSaveCallback();
            std::vector<yunuGI::Vector3> vertexList;
            std::vector<unsigned int> indexList;
            MakeUpVerticesList(vertexList, indexList, nodes);

            std::vector<Vector3f> vertexList2;
            std::vector<int> indexList2;
            vertexList2.resize(vertexList.size());
            indexList2.resize(indexList.size());
            for (auto i : vertexList)
            {
                vertexList2.push_back({ i.x, i.y, i.z });
            }
            for (auto i : indexList)
            {
                indexList2.push_back(i);
            }
            std::vector<Vector2i> affectedCoords;
            for (auto each : nodes)
            {
                affectedCoords.push_back(each.first);
            }
#ifdef _DEBUG
            ApplyDebugMesh();
#endif
            SingleNavigationField::Instance().BuildField(vertexList2, indexList2);
        }

        void TerrainData::AddNode(const Vector2i& nodeKey)
        {
            if (nodes.find(nodeKey) != nodes.end())
                return;
            auto debugMeshIdx = GetOrCreateDebugMeshIndex();
            nodes[nodeKey] = { .debugMeshIndex = debugMeshIdx };
            affectedMeshIndices.insert(debugMeshIdx);
            debugMeshes[debugMeshIdx].nodes.insert(nodeKey);
        }
        void TerrainData::EraseNode(const Vector2i& nodeKey)
        {
            if (nodes.find(nodeKey) == nodes.end())
                return;

            auto debugMeshIdx = nodes[nodeKey].debugMeshIndex;
            affectedMeshIndices.insert(debugMeshIdx);
            debugMeshes[debugMeshIdx].nodes.erase(nodeKey);
            nodes.erase(nodeKey);
        }
        void TerrainData::ClearNodes()
        {
            auto tempNodes = nodes;
            for (auto each : tempNodes)
                EraseNode(each.first);
            for (auto each : debugMeshes)
            {
                auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
                rsrcManager->DeleteMesh(each.debugMesh->GetGI().GetMesh());
                each.debugMesh->GetGI().SetMesh(nullptr);
                Scene::getCurrentScene()->DestroyGameObject(each.debugMesh->GetGameObject());
            }
            debugMeshes.clear();
            affectedMeshIndices.clear();
        }
        void TerrainData::ApplyDebugMesh()
        {
            //for (auto i = 0; i < debugMeshes.size(); i++)
            //    affectedMeshIndices.insert(i);
            for (auto each : affectedMeshIndices)
            {
                std::vector<unsigned int > indices;
                MakeUpVerticesList(debugMeshes[each].vertices, indices, debugMeshes[each].nodes);
                debugMeshes[each].normals = std::vector<yunuGI::Vector3>(debugMeshes[each].vertices.size(), { 0,1,0 });
                bool isDrawable = !debugMeshes[each].vertices.empty() && !indices.empty() && !debugMeshes[each].normals.empty();
                if (isDrawable)
                {
                    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
                    rsrcManager->DeleteMesh(debugMeshes[each].debugMesh->GetGI().GetMesh());
                    wstringstream ss;
                    ss << L"TerrainMesh : " << each;
                    auto mesh = rsrcManager->CreateMesh(ss.str().c_str(), debugMeshes[each].vertices, indices, debugMeshes[each].normals);
                    assert(mesh != nullptr);
                    debugMeshes[each].debugMesh->GetGI().SetMesh(mesh);
                }
                //if (isDrawable)
                    //debugMeshes[each].debugMesh->SetDebugGraphicsEnabled(true);
                //else
                //    debugMeshes[each].debugMesh->SetDebugGraphicsEnabled(false);
                debugMeshes[each].debugMesh->SetActive(isDrawable);
            }
            affectedMeshIndices.clear();
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
            auto mesh = AttachDebugMesh(node, DebugMeshType::Cube, yunuGI::Color{ 0.788, 0.647, 0.215 }, false);
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
                //pod.heights.push_back(each.second.height);
            }
            sort(pod.coordinates.begin(), pod.coordinates.end(),
                [](const auto& a, const auto& b) {return a.first + a.second < b.first + b.second;  });
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
            EnterDataFromGlobalConstant();
            for (auto i = 0; i < pod.coordinates.size(); ++i)
            {
                AddNode({ pod.coordinates[i].first,pod.coordinates[i].second });
            }
            std::vector<Vector2i> affectedCoords;
            for (auto each : nodes)
            {
                affectedCoords.push_back(each.first);
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
            EnterDataFromGlobalConstant();
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
        template<typename T>
        void TerrainData::MakeUpVerticesList(std::vector<yunuGI::Vector3>& vertexList, std::vector<unsigned int>& indexList, const T& nodes)
        {
            vertexList.clear();
            indexList.clear();
            for (auto& node : nodes)
            {
                Vector2i nodeKey;

                if constexpr (Has_first<std::remove_reference_t<decltype(node)>>::value)
                {
                    nodeKey = node.first;
                }
                else
                {
                    nodeKey = node;
                }
                bool toprightExists{ nodes.find(nodeKey + Vector2i{1, 1}) != nodes.end() };
                bool topExists{ nodes.find(nodeKey + Vector2i{0, 1}) != nodes.end() };
                bool rightExists{ nodes.find(nodeKey + Vector2i{1, 0}) != nodes.end() };
                if (toprightExists && topExists && rightExists)
                {
                    vertexList.push_back(GetNodePosition(nodeKey) + Vector3d::up * debugMeshYPos);
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{ 1, 0 }) + Vector3d::up * debugMeshYPos);
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{ 0, 1 }) + Vector3d::up * debugMeshYPos);
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{ 1, 1 }) + Vector3d::up * debugMeshYPos);
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 3);
                    indexList.push_back(vertexList.size() - 4);
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 1);
                    indexList.push_back(vertexList.size() - 3);
                }
                else if (toprightExists && topExists)
                {
                    vertexList.push_back(GetNodePosition(nodeKey) + Vector3d::up * debugMeshYPos);
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{ 1, 1 }) + Vector3d::up * debugMeshYPos);
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{ 0, 1 }) + Vector3d::up * debugMeshYPos);
                    indexList.push_back(vertexList.size() - 1);
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 3);
                }
                else if (toprightExists && rightExists)
                {
                    vertexList.push_back(GetNodePosition(nodeKey) + Vector3d::up * debugMeshYPos);
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{ 1, 1 }) + Vector3d::up * debugMeshYPos);
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{ 1, 0 }) + Vector3d::up * debugMeshYPos);
                    indexList.push_back(vertexList.size() - 3);
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 1);
                }
                else if (topExists && rightExists)
                {
                    vertexList.push_back(GetNodePosition(nodeKey) + Vector3d::up * debugMeshYPos);
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{ 0, 1 }) + Vector3d::up * debugMeshYPos);
                    vertexList.push_back(GetNodePosition(nodeKey + Vector2i{ 1, 0 }) + Vector3d::up * debugMeshYPos);
                    indexList.push_back(vertexList.size() - 3);
                    indexList.push_back(vertexList.size() - 2);
                    indexList.push_back(vertexList.size() - 1);
                }
            }
        }
        DebugStaticMesh* TerrainData::MakeDebugMesh()
        {
            auto debugMesh = Scene::getCurrentScene()->AddGameObject()->AddComponent<DebugStaticMesh>();
            //debugMesh->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::brown(), true));
            debugMesh->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color{ 0.5f * (debugMeshes.size() % 2), 0.5f * ((debugMeshes.size() / 2) % 2), 1, 1 }, true));
            //debugMesh->SetActive(false);
            return debugMesh;
        }
        unsigned int TerrainData::GetOrCreateDebugMeshIndex()
        {
            for (unsigned int i = 0; i < debugMeshes.size(); i++)
            {
                if (debugMeshes[i].nodes.size() < debugMeshVerticeNumThreshold)
                    return i;
            }
            debugMeshes.push_back(DebugMeshInfo{ .debugMesh = MakeDebugMesh(),.vertices{},.indices{},.nodes{} });
            return debugMeshes.size() - 1;
        }
    }
}
