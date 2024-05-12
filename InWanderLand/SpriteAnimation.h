#pragma once
#include "YunutyEngine.h"
#include <filesystem>

// Rect 사각형 위에다가 텍스처 매터리얼을 교체해주며 스프라이트 애니메이션을 재생하는 클래스입니다.
class SpriteAnimation : public graphics::StaticMeshRenderer
{
public:
    struct Sprite
    {
        yunuGI::IMaterial* material;
        double timeOffset;
    };
    void SetSprites(const wchar_t* spritesRootPath);
    static const std::vector<Sprite>* GetSprites(const wchar_t* spritesRootPath);
private:
    virtual void Start() override;
    virtual void Update() override;
    int currentIdx{ 0 };
    double elapsed{ 0 };
    const std::vector<Sprite>* spriteSheet{ nullptr };
    static std::unordered_map<std::wstring, std::vector<Sprite>> spritesMap;
};
