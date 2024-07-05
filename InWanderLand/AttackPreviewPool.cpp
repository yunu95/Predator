#include "AttackPreviewPool.h"

#include "wanderResources.h"



void AttackPreviewPool::ObjectInitializer(std::weak_ptr<graphics::StaticMeshRenderer> comp)
{
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	yunuGI::IShader* vs = _resourceManager->GetShader(L"TestDecalVS.cso");
	yunuGI::IShader* ps = _resourceManager->GetShader(L"TestDecalPS.cso");
	yunuGI::ITexture* attack = _resourceManager->GetTexture(wanderResources::texture::ATTACK_TEXTURE);
	yunuGI::IMesh* cubeMesh = _resourceManager->GetMesh(L"Cube");
	comp.lock()->GetGameObject()->GetTransform()->SetLocalScale( Vector3d{2,10,2} );
	comp.lock()->GetGI().SetMesh(cubeMesh);
	comp.lock()->GetGI().GetMaterial()->SetVertexShader(vs);
	comp.lock()->GetGI().GetMaterial()->SetPixelShader(ps);
	comp.lock()->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, attack);
}
