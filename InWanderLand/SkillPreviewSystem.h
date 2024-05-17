#pragma once
#include "YunutyEngine.h"
#include "Unit.h"

//enum class SkillPreviewMesh
//{
//	OnlyPath,
//	OnlyRange,
//	Both,
//	None
//};
//
//class SkillPreviewSystem : virtual public Component, public SingletonComponent<SkillPreviewSystem>
//{
//public:
//
//
//private:
//	Vector3d m_currentMousePosition;
//	Unit* m_currentSelectedPlayerUnit;
//
//	GameObject* m_pathPreviewObject;
//	yunutyEngine::graphics::StaticMeshRenderer* m_pathMeshComponent;
//
//	GameObject* m_rangePreviewObject;
//	yunutyEngine::graphics::StaticMeshRenderer* m_rangeMeshComponent;
//
//	SkillPreviewMesh m_currentSkillPreviewType;
//
//	bool isActivated = false;
//
//	bool isOnceRotated = false;
//	double previousDegree = 0.0f;
//
//	float currentSkillRange;
//	Unit::SkillEnum currentSkillNum;
//
//	std::unordered_map<Unit::SkillEnum, float> warriorSkillRangeInfo;
//	std::unordered_map<Unit::SkillEnum, float> magicianSkillRangeInfo;
//	std::unordered_map<Unit::SkillEnum, float> healerSkillRangeInfo;
//
//public:
//	virtual void Start() override;
//	virtual void Update() override;
//
//	void SetPathPreviewObjectRotation();
//	void SetRangePreviewObjectPosition();
//
//	void SetPathPreviewObject(GameObject* p_obj);
//	void SetRangePreviewObject(GameObject* p_obj);
//
//	void SetCurrentMousPosition(Vector3d pos);
//	void SetCurrentSelectedPlayerUnit(Unit* obj);
//	void SetCurrentSelectedSkillNum(Unit::SkillEnum p_num);
//	void SetCurrentSkillPreviewType(SkillPreviewMesh p_type);
//
//	void ActivateSkillPreview(bool p_boolen);
//
//	void SetCurrentSkillRange(float p_flt);
//	void SetDefaultSkillRange(Unit* p_unit, Unit::SkillEnum skillEnum, float p_rng);
//};

