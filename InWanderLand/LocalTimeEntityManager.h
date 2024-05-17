#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"

class LocalTimeEntity;

class LocalTimeEntityManager : public SingletonComponent<LocalTimeEntityManager>, public Component, public PermanentObservee
{
private:
	std::vector<LocalTimeEntity*> m_entityVector;

public:
	virtual void Start() override;
	virtual void OnContentsStop() override;
	virtual Component* GetComponent() override { return this; }

	void RegisterLocalTimeEntity(LocalTimeEntity* p_entity);
	void ReportTacticModeEngaged();
	void ReportTacticModeEnded();

	void SetLocalTimeScaleDirectly(LocalTimeEntity* p_entity, float p_scale);
};

