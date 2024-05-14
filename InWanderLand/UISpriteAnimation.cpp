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
    pushDuration = spriteSheet->back().timeOffset;
    onActivation = [this]()
        {
            this->currentIdx = 0;
            this->uiElement->imageComponent.lock()->GetGI().SetImage(this->spriteSheet->at(currentIdx).material->GetTexture(yunuGI::Texture_Type(yunuGI::Texture_Type::Temp0)));
        };
    onUpdate = [this](float t)
        {
            if (this->currentIdx < this->spriteSheet->size() - 1 && this->m_elapsed > spriteSheet->at(this->currentIdx).timeOffset)
            {
                this->currentIdx++;
                this->uiElement->imageComponent.lock()->GetGI().SetImage(this->spriteSheet->at(currentIdx).material->GetTexture(yunuGI::Texture_Type(yunuGI::Texture_Type::Temp0)));
            }
        };
    onCompleteFunction = [this]()
        {
            this->currentIdx = 0;
        };
}