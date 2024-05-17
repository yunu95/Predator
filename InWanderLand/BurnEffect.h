#pragma once
#include "YunutyEngine.h"
#include "YunuGraphicsInterface.h"
#include "ContentsObservee.h"

class BurnEffect : public ContentsObservee
{
public:
	void SetEdgeColor(yunuGI::Color color);
	void SetEdgeThickness(float edgeThickness);
	void SetSpeed(float speed);

	void DisAppear();
	void Appear();
	bool IsDone();

	void Start() override;
	void Update() override;

private:
	void Reset();

private:
	yunuGI::Color edgeColor{ 1,1,1,1 };

	float edgeThickness = 0.01f;
	float speed = 0.2f;
	float amount = 0.f;

	bool isFirst = true;
	bool isDone = false;
	bool isAppear = false;
	bool isDisAppear = false;

	yunutyEngine::graphics::SkinnedMesh* renderer;
	std::vector<yunuGI::IMaterial*> burnMaterialVec;
	std::vector<yunuGI::IMaterial*> originMaterialVec;
};

