#include "YunutyEngine.h"
#include "Transform.h"
#include "GameObject.h"
#include "_XMMath.h"
#include <d3d11.h>
#include <DirectXMath.h>

using namespace yunutyEngine;
using namespace DirectX;

// 문제가 많다. 캐싱에 신경을 쓰지 않아 계층 구조가 조금만 복잡해지면 부하가 매우 커진다.
Vector3d yunutyEngine::Transform::GetWorldPosition()const
{
	if (GetGameObject()->GetParentGameObject() == nullptr)
		return GetLocalPosition();

	auto deltaPosition = (GetGameObject()->GetParentGameObject()->GetTransform()->GetWorldRotation() * GetLocalPosition());
	auto parentScale = GetGameObject()->GetParentGameObject()->GetTransform()->GetWorldScale();
	deltaPosition.x *= parentScale.x;
	deltaPosition.y *= parentScale.y;
	deltaPosition.z *= parentScale.z;
	return  deltaPosition +
		GetGameObject()->GetParentGameObject()->GetTransform()->GetWorldPosition();
}
//XMMATRIX
// 문제가 많다2
Quaternion yunutyEngine::Transform::GetWorldRotation()const
{
	if (GetGameObject()->GetParentGameObject() == nullptr)
		return GetLocalRotation();
	return GetGameObject()->GetParentGameObject()->GetTransform()->GetWorldRotation() * GetLocalRotation();
}
Vector3d yunutyEngine::Transform::GetWorldScale()const
{
	if (GetGameObject()->GetParentGameObject() == nullptr)
		return GetLocalScale();
	Vector3d ret;
	auto parentScale = GetGameObject()->GetParentGameObject()->GetTransform()->GetWorldScale();
	ret.x = scale.x * parentScale.x;
	ret.y = scale.y * parentScale.y;
	ret.z = scale.z * parentScale.z;
	return ret;
}
Vector3d yunutyEngine::Transform::GetLocalPosition()const
{
	return position;
}
Quaternion yunutyEngine::Transform::GetLocalRotation()const
{
	return rotation;
}
Vector3d yunutyEngine::Transform::GetLocalScale()const
{
	return scale;
}
void yunutyEngine::Transform::SetLocalPosition(const Vector3d& position)
{
	this->position = position;
	InvokeTransformUpdateEvent(GetGameObject());
}
void yunutyEngine::Transform::SetLocalRotation(const Quaternion& rotation)
{
	this->rotation = rotation;
	InvokeTransformUpdateEvent(GetGameObject());
}
void yunutyEngine::Transform::SetLocalScale(const Vector3d& scale)
{
	this->scale = scale;
	InvokeTransformUpdateEvent(GetGameObject());
}
void yunutyEngine::Transform::InvokeTransformUpdateEvent(GameObject* obj)
{
	for (auto eachComp : obj->GetIndexedComponents())
		eachComp->OnTransformUpdate();
	for (auto eachChild : obj->GetChildren())
		InvokeTransformUpdateEvent(eachChild);
}
yunuGI::Matrix4x4 yunutyEngine::Transform::GetLocalTM()const
{
	DirectX::XMMATRIX ret = math::GetTransformMatrix(GetLocalPosition(), GetLocalScale(), GetLocalRotation());
	return reinterpret_cast<yunuGI::Matrix4x4&>(ret);
}
yunuGI::Matrix4x4 yunutyEngine::Transform::GetWorldTM()const
{
	const GameObject* parent = GetGameObject()->GetParentGameObject();
	if (!parent)
	{
		auto answer = GetLocalTM();
		return GetLocalTM();
	}

	yunuGI::Matrix4x4&& parentTM = parent->GetTransform()->GetWorldTM();
	yunuGI::Matrix4x4&& localTM = GetLocalTM();
	//DirectX::XMMATRIX&& xmOutput = reinterpret_cast<DirectX::XMMATRIX&&>(parentTM) * reinterpret_cast<DirectX::XMMATRIX&&>(localTM);
	DirectX::XMMATRIX&& xmOutput = reinterpret_cast<DirectX::XMMATRIX&&>(localTM) * reinterpret_cast<DirectX::XMMATRIX&&>(parentTM);
	yunuGI::Matrix4x4&& output = reinterpret_cast<yunuGI::Matrix4x4&&>(xmOutput);

	return output;
}
void yunutyEngine::Transform::SetWorldPosition(const Vector3d& position)
{
	auto parent = GetGameObject()->GetParentGameObject();

	if (!parent)
		SetLocalPosition(position);
	else
		SetLocalPosition(parent->GetTransform()->GetWorldRotation() * (position - parent->GetTransform()->GetWorldPosition()));
}
// 문제가 많다4
void yunutyEngine::Transform::SetWorldRotation(const Quaternion& rotation)
{
	auto parent = GetGameObject()->GetParentGameObject();

	if (!parent)
		SetLocalRotation(rotation);
	else
		SetLocalRotation(parent->GetTransform()->GetWorldRotation().inverse() * rotation);
}
void yunutyEngine::Transform::SetWorldScale(const Vector3d& scale)
{
	auto parent = GetGameObject()->GetParentGameObject();

	if (!parent)
		SetLocalScale(scale);
	else
	{
		auto parentScale = parent->GetTransform()->GetWorldScale();
		if (parentScale.x != 0 && parentScale.y != 0 && parentScale.z != 0)
		{
			Vector3d newScale = { scale.x/ parentScale.x,scale.y / parentScale.y,scale.z / parentScale.z };
			SetLocalScale(newScale);
		}
	}
}

void yunutyEngine::Transform::SetCacheDirty()
{

}