#pragma once
#include "YunutyEngine.h"
#include "Easing.h"


class Timer : public Component
{
private:
	bool m_isActivated;

public:
	bool m_isRepeated;

	float m_elapsed;
	float m_duration;
	bool isRealtime{ false };

public:
	virtual void Start() override;
	virtual void Update() override;

public:
	std::function<void(float normalizedT)> onUpdate = [](float) {};
	std::function<void()> onCompleteFunction = []() {};
	std::function<void()> onExpiration = []() {};

	void ActivateTimer();
	void StopTimer();
	void SetUsingRealtime(bool);
};