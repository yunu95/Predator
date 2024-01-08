#pragma once
#include "YunutyEngine.h"
/// <summary>
/// 만약 NavigationAgent로 인해 유닛의 위치를 파악 못하는 거면...
/// 항상 유닛의 위치를 자신의 위치로 삼는 오브젝트를 만들어 거기에 콜라이더를 붙여보자
/// </summary>
class UnitTransformComponent :  public Component
{
public:
	GameObject* ownerObject;

public:
	virtual void Update() override
	{
		GetTransform()->SetWorldPosition(ownerObject->GetTransform()->GetWorldPosition());
		GetTransform()->rotation = ownerObject->GetTransform()->rotation;
		//GetTransform()->scale = ownerObject->GetTransform()->scale;
	}
};

