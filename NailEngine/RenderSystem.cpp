#include "RenderSystem.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "NailEngine.h"
#include "Struct.h"
#include "ConstantBuffer.h"

#include "ResourceManager.h"
#include "Material.h"
#include "Mesh.h"

#include "NailCamera.h"
#include "RenderableManager.h"
#include "IRenderable.h"

#include "ILight.h"
#include "LightManager.h"

#include "NailCamera.h"

LazyObjects<RenderSystem> RenderSystem::Instance;

void RenderSystem::PushLightData()
{
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	LightParams params;
	params.lightCount = LightManager::Instance.Get().GetLightCount();

	unsigned int i = 0;

	for (auto& e : lightSet)
	{
		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Directional))
		{
			params.lights[i] = e->GetLightInfo();
			params.lights[i].direction = e->GetLightInfo().direction;
		}
		else if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Point))
		{

		}
		else if(e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Spot))
		{

		}
		
		i++;
	}

	NailEngine::Instance.Get().GetConstantBuffer(2)->PushGraphicsData(&params, sizeof(LightParams), 2);
}

void RenderSystem::PushCameraData()
{
	CameraBuffer buffer;
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	NailCamera::Instance.Get().GetWTM().Decompose(scale, quat, pos);
	buffer.position = pos;
	NailEngine::Instance.Get().GetConstantBuffer(3)->PushGraphicsData(&buffer, sizeof(CameraBuffer), 3);
}

void RenderSystem::RenderObject()
{
	PushCameraData();
	PushLightData();

	auto& renderableSet = RenderableManager::Instance.Get().GetRenderableSet();

	for (auto& e : renderableSet)
	{
		MatrixBuffer matrixBuffer;
		matrixBuffer.WTM = e->GetWorldTM();
		matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
		matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
		matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
		matrixBuffer.WorldInvTrans = e->GetWorldTM().Invert().Transpose();

		NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer),0);

		ResourceManager::Instance.Get().GetMaterial(e->GetMaterial()->GetMaterialName())->PushGraphicsData();
		ResourceManager::Instance.Get().GetMesh(e->GetMeshName())->Render();
	}
}
