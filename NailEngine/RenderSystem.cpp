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

LazyObjects<RenderSystem> RenderSystem::Instance;

void RenderSystem::PushLightData()
{
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	LightParams params;
	params.lightCount = LightManager::Instance.Get().GetLightCount();

	unsigned int i = 0;

	auto temp = sizeof(LightInfo);

	for (auto& e : lightSet)
	{
		params.lights[i] = e->GetLightInfo();
		params.lights[i].angle = 24.f;
		i++;
	}

	NailEngine::Instance.Get().GetConstantBuffer(2)->PushGraphicsData(&params, sizeof(LightParams), 2);
}

void RenderSystem::RenderObject()
{
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
