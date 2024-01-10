#include "DebugMeshes.h"

template <>
struct std::hash<yunuGI::Color>
{
    std::size_t operator()(const yunuGI::Color& k) const
    {
        using std::size_t;
        using std::hash;
        using std::string;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        return(hash<float>()(k.r) << 0)
            ^ (hash<float>()(k.g) << 1)
            ^ (hash<float>()(k.b) << 2)
            ^ (hash<float>()(k.a) << 3);
    }
};
yunuGI::IMaterial* GetColoredDebugMaterial(yunuGI::Color color)
{
    static std::unordered_map<yunuGI::Color, yunuGI::IMaterial*> materialMap;
    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    if (materialMap.find(color) != materialMap.end())
    {
        return materialMap[color];
    }
    else
    {
        wstringstream ss{ L"ColorCached DebugMaterial " };
        ss << materialMap.size();
        auto ret = materialMap[color] = rsrcManager->CreateMaterial(ss.str());
        ret->SetColor(color);
        return ret;
    }
}
