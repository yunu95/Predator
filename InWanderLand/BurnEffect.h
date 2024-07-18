#pragma once
#include "YunutyEngine.h"
#include "YunuGraphicsInterface.h"
#include "ContentsObservee.h"

class BurnEffect : public Component
{
public:
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
	void Pause();
	void Resume();

private:
	float duration = 0.2f;
	float amount = 0.f;
	float defaultStart = 0.9f;
	float defaultEnd = -2.7f;
	float accTime = 0.f;
	float uv = 0.f;

	bool isFirst = true;
	bool isDone = false;
	bool isAppear = false;
	bool isDisAppear = false;
	bool isPause = false;

	yunutyEngine::graphics::SkinnedMesh* renderer;
	std::vector<yunuGI::IMaterial*> burnMaterialVec;
	std::vector<yunuGI::IMaterial*> originMaterialVec;
};

