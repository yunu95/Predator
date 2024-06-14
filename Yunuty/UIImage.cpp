#include "UIImage.h"
#include "YunutyEngine.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::UIImage::UIImage() :
    Renderable<yunuGI::IUIImage>(_YunuGIObjects::SingleInstance().factory->CreateUIImage({}))
{
}
void yunutyEngine::graphics::UIImage::OnTransformUpdate()
{
    GetGI().SetWorldTM(GetTransform()->GetWorldTM());
    GetGI().SetRotation(GetTransform()->GetWorldRotation().Euler().z);
}