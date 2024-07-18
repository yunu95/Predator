#include "InWanderLand.h"
#include "DebugMeshes.h"
#include "EditorResourceManager.h"
#include "DebugStaticMesh.h"

yunuGI::IMaterial* GetColoredDebugMaterial(yunuGI::Color color, bool isWireFrame)
{
    static std::unordered_map<yunuGI::Color, yunuGI::IMaterial*> defaultMaterialMap;
    static std::unordered_map<yunuGI::Color, yunuGI::IMaterial*> wireFramedMaterialMap;
    std::unordered_map<yunuGI::Color, yunuGI::IMaterial*>& materialMap = isWireFrame ? wireFramedMaterialMap : defaultMaterialMap;
    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    if (materialMap.find(color) != materialMap.end())
    {
        return materialMap[color];
    }
    else
    {
        wstringstream ss{ isWireFrame ? L"wireframed ColorCached DebugMaterial " : L"default ColorCached DebugMaterial " };
        ss << materialMap.size();
        auto ret = materialMap[color] = rsrcManager->CreateMaterial(ss.str());
        ret->SetColor(color);
        auto& shaderList = rsrcManager->GetShaderList();
        yunuGI::IShader* shader = nullptr;
        for (auto each : shaderList)
        {
            if (isWireFrame && each->GetName() == L"DebugPS.cso")
            {
                shader = each;
            }
            if (!isWireFrame && each->GetName() == L"Debug_AlphaPS.cso")
            {
                shader = each;
            }
        }
        ret->SetPixelShader(shader);
        return ret;
    }
}
yunuGI::IMaterial* GetColoredDebugMaterialTransparent(yunuGI::Color color)
{
    static std::unordered_map<yunuGI::Color, yunuGI::IMaterial*> materialMap;
    auto& editorRsrcManager = application::editor::ResourceManager::GetSingletonInstance();
    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

    if (materialMap.find(color) != materialMap.end())
    {
        return materialMap[color];
    }
    else
    {
        wstringstream ss{ L"transparent ColorCached DebugMaterial " };
        ss << materialMap.size();
        auto ret = materialMap[color] = rsrcManager->CreateMaterial(ss.str());
        ret->SetColor(color);
        ret->SetPixelShader(editorRsrcManager.GetShader("Debug_AlphaPS.cso"));
        return ret;
    }
}
inline yunutyEngine::graphics::StaticMeshRenderer* AttachDebugMesh(GameObject* target, DebugMeshType meshType, yunuGI::Color color, bool isWireFrame)
{
    // AddComponent가 여기서 터진 횟수 1회
    auto staticMesh = target->AddComponent<DebugStaticMesh>();
    wstring meshName;
    switch (meshType)
    {
    case DebugMeshType::Line:
        // Line은 기본 트랜스폼으로 배치되었을 시 0,0,0을 시점으로, 1,0,0을 종점으로 한다.
        meshName = L"Line";
        break;
    case DebugMeshType::Cube:
        meshName = L"Cube";
        break;
    case DebugMeshType::Sphere:
        meshName = L"Sphere";
        break;
    case DebugMeshType::Rectangle:
        meshName = L"Rectangle";
        break;
    case DebugMeshType::Capsule:
        meshName = L"Capsule";
        break;
    }
    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    staticMesh->GetGI().SetMesh(rsrcManager->GetMesh(meshName));
    staticMesh->GetGI().SetMaterial(0, GetColoredDebugMaterial(color, isWireFrame));
    return staticMesh;
}
inline yunutyEngine::graphics::StaticMeshRenderer* CreateLine(Vector3d start, Vector3d end, yunuGI::Color color)
{
    if (start == end)
    {
        return nullptr;
    }

    auto gameObject = Scene::getCurrentScene()->AddGameObject();
    auto delta = (end - start);
    gameObject->GetTransform()->SetLocalScale({ delta.Magnitude(), 1, 1 });

    Vector3d axis = Vector3d::Cross(Vector3d(1, 0, 0), delta).Normalized();
    if (axis == Vector3d(0, 0, 0))
    {
        axis = Vector3d(0, 0, 1);
    }

    gameObject->GetTransform()->SetWorldRotation(Quaternion::MakeAxisAngleQuaternion(axis, acos(Vector3d::Dot(Vector3d(1, 0, 0), delta.Normalized()))));
    gameObject->GetTransform()->SetWorldPosition(start);
    return AttachDebugMesh(gameObject, DebugMeshType::Line, color, true);
}