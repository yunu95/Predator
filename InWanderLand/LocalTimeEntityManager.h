#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"

class LocalTimeEntity;

class LocalTimeEntityManager : public Component, public SingletonComponent<LocalTimeEntityManager>, public PermanentObservee
{
private:
	std::vector<LocalTimeEntity*> m_entityVector;

public:
	virtual void Start() override;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;

	void RegisterLocalTimeEntity(LocalTimeEntity* p_entity);
	void ReportTacticModeEngaged();
	void ReportTacticModeEnded();

	void SetLocalTimeScaleDirectly(LocalTimeEntity* p_entity, float p_scale);
};

