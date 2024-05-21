#pragma once
#include "YunutyEngine.h"
#include "YunuGraphicsInterface.h"

class UVAnimator : public yunutyEngine::Component
{
public:
	void SetStaticMeshRenderer(yunutyEngine::graphics::StaticMeshRenderer* renderer);
	void SetMesh(yunuGI::IMesh* mesh);
	void SetTexture(yunuGI::ITexture* texture);
	void SetDirection(yunutyEngine::Vector2d direction);
	void SetSpeed(float speed);

	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDisable() override;


private:

private:
	yunutyEngine::graphics::StaticMeshRenderer* renderer;

	float speed = 1;
	yunutyEngine::Vector2d offset;
	yunutyEngine::Vector2d direction{-1.f,0.f};
};

