#include "MoveEndPreviewPool.h"

#include "wanderResources.h"



void MoveEndPreviewPool::ObjectInitializer(std::weak_ptr<graphics::StaticMeshRenderer> comp)
{
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	yunuGI::IShader* vs = _resourceManager->GetShader(L"TestDecalVS.cso");
	yunuGI::IShader* ps = _resourceManager->GetShader(L"TestDecalPS.cso");
	yunuGI::ITexture* move = _resourceManager->GetTexture(wanderResources::texture::MOVE_END_TEXTURE);
	yunuGI::IMesh* cubeMesh = _resourceManager->GetMesh(L"Cube");
	comp.lock()->GetGameObject()->GetTransform()->SetLocalScale(Vector3d{2,10,2} );
	comp.lock()->GetGI().SetMesh(cubeMesh);
	comp.lock()->GetGI().GetMaterial()->SetVertexShader(vs);
	comp.lock()->GetGI().GetMaterial()->SetPixelShader(ps);
	comp.lock()->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, move);
}
