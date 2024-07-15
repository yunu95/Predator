#pragma once
#include "YunutyEngine.h"
#include "YunuGraphicsInterface.h"
#include "ContentsObservee.h"

class BurnEffect : public Component
{
public:
	void SetEdgeColor(yunuGI::Color color);
	void SetEdgeThickness(float edgeThickness);
	void SetDuration(float duration);

	void Init();
	void Disappear();
	void Appear();
	bool IsDone();

	void OnEnable() override;
	void OnDisable() override;
	void Start() override;
	void Update() override;

	void Reset();

private:
	yunuGI::Color edgeColor{ 1,1,1,1 };

	float edgeThickness = 0.01f;
	float duration = 0.2f;
	float amount = 0.f;

	bool isFirst = true;
	bool isDone = false;
	bool isAppear = false;
	bool isDisAppear = false;

	yunutyEngine::graphics::SkinnedMesh* renderer;
	std::vector<yunuGI::IMaterial*> burnMaterialVec;
	std::vector<yunuGI::IMaterial*> originMaterialVec;
};

