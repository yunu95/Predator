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

LazyObjects<RenderSystem> RenderSystem::Instance;

void RenderSystem::RenderObject()
{
	auto& renderableSet = RenderableManager::Instance.Get().GetRenderableSet();

	for (auto& e : renderableSet)
	{
		MatrixBuffer matrixBuffer;
		matrixBuffer.WTM = e->GetWorldTM();
		matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
		matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
		matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;

		NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer),0);

		ResourceManager::Instance.Get().GetMaterial(e->GetMaterial()->GetMaterialName())->PushGraphicsData();
		ResourceManager::Instance.Get().GetMesh(e->GetMeshName())->Render();
	}
}
