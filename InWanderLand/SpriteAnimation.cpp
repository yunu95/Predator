#include "SpriteAnimation.h"
#include <regex>

std::unordered_map<std::wstring, std::vector<SpriteAnimation::Sprite>> SpriteAnimation::spritesMap{};
void SpriteAnimation::SetSprites(const wchar_t* spritesRootPath)
{
    if (auto itr = spritesMap.find(spritesRootPath); itr != spritesMap.end())
    {
        spriteSheet = &itr->second;
        return;
    }
    spritesMap[spritesRootPath] = std::vector<Sprite>();
    spriteSheet = &spritesMap[spritesRootPath];
    namespace fs = std::filesystem;
    fs::path basePath{ L"./" };
    fs::path spritesRoot{ spritesRootPath };
    auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    try
    {
        if (fs::exists(spritesRoot) && fs::is_directory(spritesRoot))
        {
            double timeOffset{ 0 };
            for (const auto& entry : fs::directory_iterator(spritesRoot))
            {
                if (fs::is_regular_file(entry))
                {
                    wstring relativePath = fs::relative(entry.path(), basePath);
                    wstring matName = L"spritesAnim/" + relativePath;
                    auto newMat = rsrcMgr->CreateMaterial(matName);
                    auto texture = rsrcMgr->GetTexture(relativePath);
                    newMat->SetTexture(yunuGI::Texture_Type::Temp0, texture);
                    newMat->SetVertexShader(rsrcMgr->GetShader(L"TextureVS.cso"));
                    newMat->SetPixelShader(rsrcMgr->GetShader(L"TexturePS.cso"));
                    wsmatch match;
                    double interval = 0;
                    std::regex_match(relativePath, match, wregex(L".*?(\\d+)ms.*"));
                    if (match.size() >= 2)
                        interval = 0.001 * stoi(match[1]);
                    if (interval <= 0)
                        interval = 0.1;
                    spritesMap[spritesRootPath].push_back({ newMat, timeOffset });
                    timeOffset += interval;
                }
            }
        }
    }
    catch (const fs::filesystem_error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
    }
}
void SpriteAnimation::Start()
{
    graphics::StaticMeshRenderer::Start();
    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    GetGI().SetMesh(rsrcManager->GetMesh(L"Rectangle"));
}
void SpriteAnimation::Update()
{
    if (spriteSheet)
    {
        elapsed += yunutyEngine::Time::GetDeltaTime();
        if (elapsed > spriteSheet->at(currentIdx).timeOffset)
        {
            elapsed = 0;
            currentIdx = (currentIdx + 1) % spriteSheet->size();
            GetGI().SetMaterial(0, spriteSheet->at(currentIdx).material);
        }
    }
}
