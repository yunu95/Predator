#include "MoveEndPreviewPool.h"

#include "wanderResources.h"



void MoveEndPreviewPool::ObjectInitializer(std::weak_ptr<graphics::StaticMeshRenderer> comp)
{
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	yunuGI::IShader* vs = _resourceManager->GetShader(L"TextureVS.cso");
	yunuGI::IShader* ps = _resourceManager->GetShader(L"GuideLinePS.cso");
	yunuGI::ITexture* move = _resourceManager->GetTexture(wanderResources::texture::MOVE_END_TEXTURE);
	yunuGI::IMesh* quadMesh = _resourceManager->GetMesh(L"Rectangle");
	comp.lock()->GetGameObject()->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
	comp.lock()->GetGI().GetMaterial()->SetVertexShader(vs);
	comp.lock()->GetGI().GetMaterial()->SetPixelShader(ps);
	comp.lock()->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, move);
	comp.lock()->GetGI().SetMesh(quadMesh);
}
