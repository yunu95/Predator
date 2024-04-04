#pragma once
#include "IYunuGIStaticMeshRenderer.h"
#include "IYunuGISkinnedMesh.h"
#include "IYunuGIParticleRenderer.h"
#include "IYunuGIDirectionalLight.h"
#include "IYunuGIPointLight.h"
#include "IYunuGICamera.h"
#include "IYunuGIDebugCube.h"
#include "IYunuGIUIImage.h"
#include "IYunuGIUIText.h"
#include "IAnimator.h"
#include "IUnknown.h"

// 팩토리 객체는 카메라, 메쉬와 같이 렌더링에 필요한 객체들을 생성한다.
namespace yunuGI
{
    struct IFactoryDesc
    {
    };
    class IFactory : public yunuGI::IUnknown
    {
    public:
        virtual yunuGI::ICamera* CreateCamera(const yunuGI::ICameraDesc& desc) = 0;
        virtual yunuGI::IStaticMeshRenderer* CreateStaticMesh(const yunuGI::IStaticMeshRendererDesc& desc) = 0;
        virtual yunuGI::ISkinnedMesh* CreateSkinnedMesh(const yunuGI::ISkinnedMeshDesc& desc)=0;
        virtual yunuGI::IParticleRenderer* CreateParticleRenderer(const yunuGI::IParticleRendererDesc& desc)=0;
        virtual yunuGI::IDebugCube* CreateDebugCube(const yunuGI::IDebugCubeDesc& desc) = 0;
        virtual yunuGI::IUIImage* CreateUIImage(const yunuGI::IUIImageDesc& desc) = 0;
        virtual yunuGI::IUIText* CreateUIText(const yunuGI::IUITextDesc& desc) = 0;
        virtual yunuGI::IDirectionalLight* CreateDirectionalLight(const yunuGI::IDirectionalLightDesc& desc) = 0;
        virtual yunuGI::IPointLight* CreatePointLight(const yunuGI::IPointLightDesc& desc) = 0;
        virtual yunuGI::ISpotLight* CreateSpotLight(const yunuGI::ISpotLightDesc& desc) = 0;
        virtual yunuGI::IAnimator* CreateAnimator(const yunuGI::IAnimatorDesc& desc) = 0;
    };
}
