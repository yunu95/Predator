#pragma once
#include "UISpriteAnimation.h"
#include "SpriteAnimation.h"
#include "UIImage.h"
#include "InWanderLand.h"

void UISpriteAnimation::SetSprites(const wchar_t* spritesRootPath)
{
    spriteSheet = SpriteAnimation::GetSprites(spritesRootPath);
}
void UISpriteAnimation::Init()
{
    isRealtime = true;
    duration = spriteSheet->back().timeOffset;
    onActivation = [this]()
        {
            this->currentIdx = 0;
            imageComponent.lock()->GetGI().SetImage(this->spriteSheet->at(currentIdx).material->GetTexture(yunuGI::Texture_Type(yunuGI::Texture_Type::Temp0)));
        };
    onUpdate = [this](float t)
        {
            if (this->currentIdx < this->spriteSheet->size() - 1 && this->m_elapsed > spriteSheet->at(this->currentIdx).timeOffset)
            {
                this->currentIdx++;
                imageComponent.lock()->GetGI().SetImage(this->spriteSheet->at(currentIdx).material->GetTexture(yunuGI::Texture_Type(yunuGI::Texture_Type::Temp0)));
            }
        };
    onCompleteFunction = [this]()
        {
            this->currentIdx = 0;
        };
}