#pragma once
//#include "YunuGICameraAdapter.h"
#include "YunuGraphicsInterface.h"
#include "YGCameraAdapter.h"
#include "YGStaticMeshAdapter.h"
#include "YGSkinnedMeshAdapter.h"
#include "ParticleRendererAdapter.h"
#include "YGDebugCubeAdapter.h"
#include "YGUIImageAdapter.h"
#include "YGUITextAdapter.h"
#include "YGUIImageAdapter.h"
#include "YGUITextAdapter.h"
#include "YGDebugCubeAdapter.h"
#include "IYunuGIDebugCube.h"
#include "IYunuGIUIImage.h"
#include "IYunuGIDirectionalLight.h"
#include "YGDirectionalLightAdapter.h"
#include "IYunuGIPointLight.h"
#include "YGPointLightAdapter.h"
#include "IYunuGISpotLight.h"
#include "YGSpotLightAdapter.h"
#include "YGAnimatorAdapter.h"

namespace yunuGIAdapter
{
	class FactoryAdapter : public yunuGI::IFactory
	{
	public:
		virtual yunuGI::ICamera* CreateCamera(const yunuGI::ICameraDesc& desc) {
			return new yunuGIAdapter::CameraAdapter();
		}
		virtual yunuGI::IStaticMeshRenderer* CreateStaticMesh(const yunuGI::IStaticMeshRendererDesc& desc)
		{
			return new StaticMeshAdapter{};
		}
		virtual yunuGI::ISkinnedMesh* CreateSkinnedMesh(const yunuGI::ISkinnedMeshDesc& desc)
		{
			return new SkinnedMeshAdapter{};
		}
		virtual yunuGI::IParticleRenderer* CreateParticleRenderer(const yunuGI::IParticleRendererDesc& desc)
		{
			return new ParticleRendererAdapter{};
		}
		virtual yunuGI::IDirectionalLight* CreateDirectionalLight(const yunuGI::IDirectionalLightDesc& desc)
		{
			return new DirectionalLightAdapter{};
		}
		virtual yunuGI::IPointLight* CreatePointLight(const yunuGI::IPointLightDesc& desc)
		{
			return new PointLightAdapter{};
		}
		virtual yunuGI::ISpotLight* CreateSpotLight(const yunuGI::ISpotLightDesc& desc)
		{
			return new SpotLightAdapter{};
		}
		virtual yunuGI::IDebugCube* CreateDebugCube(const yunuGI::IDebugCubeDesc& desc)
		{
			return new DebugCubeAdapter{};
		}
		virtual yunuGI::IUIImage* CreateUIImage(const yunuGI::IUIImageDesc& desc)
		{
			return new UIImageAdapter{};
		}
		virtual yunuGI::IUIText* CreateUIText(const yunuGI::IUITextDesc& desc)
		{
			return new UITextAdapter{};
		}
		virtual yunuGI::IAnimator* CreateAnimator(const yunuGI::IAnimatorDesc& desc)
		{
			return new AnimatorAdapter{};
		}
	};
}