#include "SingletonInstanceContainer.h"
#include "PlayerController.h"
#include "AutoAttackProjectilePool.h"
#include "EnemyAutoAttackProjectilePool.h"
#include "HealerAutoAttackProjectilePool.h"
#include "MagicianAutoAttackProjectilePool.h"
#include "StatusTimerPool.h"
#include "DebuggingMeshPool.h"
#include "TacticModeSystem.h"

void SingletonInstanceContainer::ClearLazySingletonInstances()
{
	PlayerController::SingleInstance().ProhibitCreateInstance();
	AutoAttackProjectilePool::SingleInstance().ProhibitCreateInstance();
	EnemyAutoAttackProjectilePool::SingleInstance().ProhibitCreateInstance();
	HealerAutoAttackProjectilePool::SingleInstance().ProhibitCreateInstance();
	MagicianAutoAttackProjectilePool::SingleInstance().ProhibitCreateInstance();
	StatusTimerPool::SingleInstance().ProhibitCreateInstance();
	DebuggingMeshPool::SingleInstance().ProhibitCreateInstance();
	TacticModeSystem::SingleInstance().ProhibitCreateInstance();

	PlayerController::SingleInstance().InitInstance();
	AutoAttackProjectilePool::SingleInstance().InitInstance();
	EnemyAutoAttackProjectilePool::SingleInstance().InitInstance();
	HealerAutoAttackProjectilePool::SingleInstance().InitInstance();
	MagicianAutoAttackProjectilePool::SingleInstance().InitInstance();
	StatusTimerPool::SingleInstance().InitInstance();
	DebuggingMeshPool::SingleInstance().InitInstance();
	TacticModeSystem::SingleInstance().InitInstance();

}

void SingletonInstanceContainer::PermitCreateInstances()
{
	PlayerController::SingleInstance().PermitCreateInstance();
	AutoAttackProjectilePool::SingleInstance().PermitCreateInstance();
	EnemyAutoAttackProjectilePool::SingleInstance().PermitCreateInstance();
	HealerAutoAttackProjectilePool::SingleInstance().PermitCreateInstance();
	MagicianAutoAttackProjectilePool::SingleInstance().PermitCreateInstance();
	StatusTimerPool::SingleInstance().PermitCreateInstance();
	DebuggingMeshPool::SingleInstance().PermitCreateInstance();
	TacticModeSystem::SingleInstance().PermitCreateInstance();
}
