#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"

/// <summary>
/// 피해를 받을 경우, 어떤 피해를 받았는지 알 수 있도록 다른 색의 큐브를 띄워준다.
/// </summary>

class Unit;

enum class MaterialNum
{
	Red,
	Green,
};

class DebuggingMesh : public ContentsObservee
{
public:
	void SetUnitObject(Unit* p_unit);
	void PopMeshUP(yunuGI::Color p_color, MaterialNum p_matNum);

private:
	GameObject* m_ownerObject;
	yunutyEngine::graphics::StaticMeshRenderer* m_staticMeshRendererComp;

	bool isPopStarted = false;

	float m_floatingElapsed = 0.0f;
	float m_floatingTime = 2.0f;
	
	float m_Yincreasement;
	float m_floatingPerFrame = 0.15f;

public:
	virtual void Start() override;
	virtual void Update() override;
	friend class DebuggingMeshPool;
};

