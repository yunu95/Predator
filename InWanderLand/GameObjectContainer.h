#pragma once
#include "YunutyEngine.h"
/// <summary>
/// 게임 오브젝트를 생성한 후 등록이 가능하다.
/// </summary>
class GameObjectContainer : SingletonClass<GameObjectContainer>
{
private:
	std::vector<GameObject*> objectVector;
public:
	void RegisterObject(GameObject* p_obj);
	void ClearVector();
};

