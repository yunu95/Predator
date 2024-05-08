#pragma once
#include "LocalTimeEntityManager.h"

class LocalTimeEntity
{
public:
	LocalTimeEntity();

protected:
	float m_localTimeScale{ 1.0f };			/// DeltaTIme 에 곱해줄 Local Time Scale

public:
	void SetLocalTimeScale(float p_timeScale);
};

