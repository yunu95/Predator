#pragma once
#include "YunutyEngine.h"


class StatusTimer : public Component
{
private:
	bool m_isActivated;

public:
	bool m_isRepeated;

	float m_elapsed;
	float m_duration;


public:
	virtual void Start() override;
	virtual void Update() override;

public:
	std::function<void()> onUpdate = []() {};
	std::function<void()> onCompleteFunction = []() {};		
	std::function<void()> onExpiration = []() {};			

	void ActivateTimer();
	void StopTimer();
};

