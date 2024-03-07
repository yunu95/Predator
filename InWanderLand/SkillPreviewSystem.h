#pragma once
#include "YunutyEngine.h"

/// <summary>
/// InputManager에서 플레이어의 스킬 키를 입력받았을 때,
/// Player Unit - Mouse Cursor 사이에 스킬 경로 및 범위를 표시해주는 컴포넌트.
/// yunutyEngine::graphics::StaticMeshRenderer는 나중에 아트 리소스로 교체해주자.
/// </summary>

class SkillPreviewSystem : public SingletonClass<SkillPreviewSystem>
{
public:
	enum class SkillPreviewMesh
	{
		OnlyPath,
		OnlyRange,
		Both,
		None
	};

private:
	Vector3d m_currentMousePosition;
	GameObject* m_currentSelectedPlayerObject;

	GameObject* m_pathPreviewObject;
	yunutyEngine::graphics::StaticMeshRenderer* m_pathMeshComponent;

	GameObject* m_rangePreviewObject;
	yunutyEngine::graphics::StaticMeshRenderer* m_rangeMeshComponent;

	SkillPreviewMesh m_currentSkillPreviewType;

	bool isActivated = false;

	bool isOnceRotated = false;
	double previousDegree = 0.0f;

public:
	void Update();

	void SetPathPreviewObjectRotation();
	void SetRangePreviewObjectPosition();

	void SetPathPreviewObject(GameObject* p_obj);
	void SetRangePreviewObject(GameObject* p_obj);

	void SetCurrentMousPosition(Vector3d pos);
	void SetCurrentSelectedPlayerGameObject(GameObject* obj);
	void SetCurrentSkillPreviewType(SkillPreviewSystem::SkillPreviewMesh p_type);

	void ActivateSkillPreview(bool p_boolen);
};

