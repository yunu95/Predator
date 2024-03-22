#include "InWanderLand.h"
#include "DebugMeshes.h"
#include "EditorResourceManager.h"

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
