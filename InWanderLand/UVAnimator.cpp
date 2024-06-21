#include "UVAnimator.h"



void UVAnimator::SetStaticMeshRenderer(yunutyEngine::graphics::StaticMeshRenderer* renderer)
{
	this->renderer = renderer;
}

void UVAnimator::SetMesh(yunuGI::IMesh* mesh)
{
	if (this->renderer)
	{
		this->renderer->GetGI().SetMesh(mesh);
	}
}

void UVAnimator::SetTexture(yunuGI::ITexture* texture)
{
	if (this->renderer)
	{
		this->renderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, texture);
	}
}

void UVAnimator::SetDirection(yunutyEngine::Vector2d direction)
{
	this->direction = direction;
}

void UVAnimator::SetSpeed(float speed)
{
	this->speed = speed;
}

void UVAnimator::Start()
{
	if (this->renderer)
	{
		if (this->defaultVal)
		{
			auto resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
			this->renderer->GetGI().GetMaterial()->SetVertexShader(resourceManager->GetShader(L"TextureAnimVS.cso"));
			this->renderer->GetGI().GetMaterial()->SetPixelShader(resourceManager->GetShader(L"TextureAnimPS.cso"));
			//this->renderer->GetGI().SetMesh(resourceManager->GetMesh(L"Rectangle"));
		}
	}
}

void UVAnimator::Update()
{
	if (this->renderer)
	{
		offset.x += direction.x * this->speed * Time::GetDeltaTime();
		offset.y += direction.y * this->speed * Time::GetDeltaTime();
		this->renderer->GetGI().GetMaterial()->SetFloat(0, offset.x);
		this->renderer->GetGI().GetMaterial()->SetFloat(1, offset.y);
	}
}

void UVAnimator::OnDisable()
{
	offset = yunutyEngine::Vector2d{};
}