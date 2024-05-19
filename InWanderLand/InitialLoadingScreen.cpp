#include "InitialLoadingScreen.h"
#include "InWanderLand.h"
#include "VideoPlayer.h"

void InitialLoadingScreen::Start()
{
    StartCoroutine(ShowLoadingScreen());
}
coroutine::Coroutine InitialLoadingScreen::ShowLoadingScreen()
{
    auto videoObj = Scene::getCurrentScene()->AddGameObject();
    auto uiImage = videoObj->AddComponent<graphics::UIImage>();
    auto videoPlayer = videoObj->AddComponent<graphics::VideoPlayer>();
    uiImage->GetGI().SetWidth(1000);
    uiImage->GetGI().SetHeight(500);
    uiImage->GetGI().SetVideo(L"Texture/LoadingScreen/NailEngine.mp4");
    videoPlayer->SetVideo(graphics::Renderer::SingleInstance().GetResourceManager()->GetVideoData(L"Texture/LoadingScreen/NailEngine.mp4"));

    //wanderUtils::ResourceRecursiveLoader::Load("Texture/LoadingScreen/");
    //auto spriteAnimObj = Scene::getCurrentScene()->AddGameObject();
    //auto spriteAnim = spriteAnimObj->AddComponent<UISpriteAnimation>();
    //spriteAnim->SetSprites(L"Texture/LoadingScreen/NailLogo");
    //spriteAnim->imageComponent = spriteAnimObj->AddComponentAsWeakPtr<graphics::UIImage>();
    //spriteAnim->imageComponent.lock()->GetGI().SetWidth(1920);
    //spriteAnim->imageComponent.lock()->GetGI().SetHeight(1080);
    //spriteAnim->Init();
    //spriteAnim->ActivateTimer();
    //while (spriteAnim->GetActive())
    //{
    //    co_await std::suspend_always{};
    //}
    co_return;
}
