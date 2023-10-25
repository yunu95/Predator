#include "Bullet.h"
#include "GameObject.h"
#include "Scene.h"
#include "BoxCollider2D.h"
#include "StaticMeshRenderer.h"
#include "YunuGIColor.h"
#include "YunutyTime.h"
#include "ProjectilePool.h"
#include "CommonUnit.h"
#include "Timer.h"
#include "RTSCam.h"
using namespace yunutyEngine;

Bullet::Bullet()
{
	m_width = 1.0;
	m_height = 0.5;

	destroyAnimTimer.duration = 0.5;
	destroyAnimTimer.onTimerExpiration = [this]()
	{
		m_isDestroyTimerOn = false;
		ProjectilePool<Bullet>::GetInstance()->RetrieveProjectile(this);
	};
}

void Bullet::CreateProjectile()
{
	// Bullet Pool이 없으면 만들어준다.
	if (ProjectilePool<Bullet>::GetInstance() == nullptr)
	{
		yunutyEngine::GameObject* bulletPool = Scene::getCurrentScene()->AddGameObject();
		bulletPool->AddComponent<ProjectilePool<Bullet>>();
	}

	// Bullet 생성 후 Pool에 넣어준다.
	yunutyEngine::GameObject* projectile = Scene::getCurrentScene()->AddGameObject();
	projectile->AddComponent<Bullet>()->Initialize();
	ProjectilePool<Bullet>::GetInstance()->RetrieveProjectile(projectile->GetComponent<Bullet>());
}

void Bullet::Initialize()
{
	// Bullet Collider box 생성
	GetGameObject()->AddComponent<BoxCollider2D>()->SetWidth(m_width);
	GetGameObject()->GetComponent<BoxCollider2D>()->SetHeight(m_height);
#if defined(DEBUG) || defined(_DEBUG)
	// Hitbox Debug Cube
	auto childObject = GetGameObject()->AddGameObject();
	auto hixBoxRect = childObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	hixBoxRect->GetGI().LoadMesh("Cube");
	hixBoxRect->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0,1,0,0 });
	hixBoxRect->GetGI().GetMaterial()->SetPixelShader(L"DebugPS.cso");
	childObject->GetTransform()->scale = Vector3d(m_width, m_height, 0);
#endif
}

void Bullet::Destroy()
{
	// Destroy Animation Timer On
	if (!m_isDestroyTimerOn)
	{
		destroyAnimTimer.Start();
		// Load Animation
		//if (isPlayerProjectile)
		//	animatedSprite->LoadAnimationFromFile(L"sprites/projectile/bullet/player/destroy");
		//else
		//	animatedSprite->LoadAnimationFromFile(L"sprites/projectile/bullet/enemy/destroy");
	}
	m_isDestroyTimerOn = true;
}

void Bullet::Update()
{
	if (m_isDestroyTimerOn) // 총알이 대상에 닿아서 타이머가 켜졌을 때
	{
		destroyAnimTimer.Update(); // 총알 파괴 애니메이션 재생
	}
	else
	{
		// 대상에 닿지 않았다면 계속 움직인다.
		GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + speed * Time::GetDeltaTime());
		GetTransform()->SetWorldRotation(Vector3d(0, 0, Vector2d(speed).GetAngleDegree()));
	}
}

void Bullet::OnCollisionEnter2D(const yunutyEngine::Collision2D& collision)
{
	if (isPlayerProjectile) // 플레이어가 쏜 총알일 때
	{
		if (collision.m_OtherCollider->GetTag() == "Hitbox")
		{
			auto target = collision.m_OtherCollider->GetGameObject()->GetComponent<CommonUnit>();
			if (target != nullptr)
			{
				if (target->GetIsPlayer() == false) // 닿은 대상이 적이라면
				{
					target->Damaged(damage);
					Destroy();
					return;
				}
			}
		}
	}
	else // 적이 쏜 총알일 때
	{
		if (collision.m_OtherCollider->GetTag() == "Hitbox")
		{
			auto target = collision.m_OtherCollider->GetGameObject()->GetComponent<CommonUnit>();
			if (target != nullptr)
			{
				if (target->GetIsPlayer() == true) // 닿은 대상이 플레이어라면
				{
					target->Damaged(damage);
					Destroy();
					return;
				}
			}
		}
	}
}

void Bullet::OnCollisionExit2D(const yunutyEngine::Collision2D& collision)
{
	// 화면 밖을 빠져나가면 Pool에 다시 넣어준다.
	auto target = collision.m_OtherCollider->GetGameObject()->GetComponent<RTSCam>();
	if(target != nullptr)
	{
		ProjectilePool<Bullet>::GetInstance()->RetrieveProjectile(this);
	}
}
