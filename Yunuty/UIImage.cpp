#include "UIImage.h"
#include "YunutyEngine.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::UIImage::UIImage() :
    Renderable<yunuGI::IUIImage>(_YunuGIObjects::SingleInstance().factory->CreateUIImage({}))
{
    GetGI().SetActive(GetActive());
}
void yunutyEngine::graphics::UIImage::OnEnable()
{
    GetGI().SetActive(true);
}
void yunutyEngine::graphics::UIImage::OnDisable()
{
    GetGI().SetActive(false);
}
void yunutyEngine::graphics::UIImage::OnTransformUpdate()
{
    auto scale = GetTransform()->GetWorldScale();
    GetGI().SetWorldTM(GetTransform()->GetWorldTM());
    GetGI().SetRotation(GetTransform()->GetWorldRotation().Euler().z);
    GetGI().SetXScale(scale.x);
    GetGI().SetYScale(scale.y);
}