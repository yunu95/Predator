#include "SingletonInstanceContainer.h"
#include "PlayerController.h"
#include "AutoAttackProjectilePool.h"
#include "EnemyAutoAttackProjectilePool.h"
#include "HealerAutoAttackProjectilePool.h"
#include "MagicianAutoAttackProjectilePool.h"
#include "StatusTimerPool.h"
#include "DebuggingMeshPool.h"
#include "TacticModeSystem.h"
#include "MeleeEnemyPool.h"
#include "RangedEnemyPool.h"
#include "LazySingletonClass.h"
#include "PassiveCakePool.h"

void SingletonInstanceContainer::ClearLazySingletonInstances()
{
	PlayerController::SingleInstance().ProhibitCreateInstance();
	MeleeEnemyPool::SingleInstance().ProhibitCreateInstance();
	RangedEnemyPool::SingleInstance().ProhibitCreateInstance();
	AutoAttackProjectilePool::SingleInstance().ProhibitCreateInstance();
	EnemyAutoAttackProjectilePool::SingleInstance().ProhibitCreateInstance();
	HealerAutoAttackProjectilePool::SingleInstance().ProhibitCreateInstance();
	MagicianAutoAttackProjectilePool::SingleInstance().ProhibitCreateInstance();
	StatusTimerPool::SingleInstance().ProhibitCreateInstance();
	DebuggingMeshPool::SingleInstance().ProhibitCreateInstance();
	PassiveCakePool::SingleInstance().ProhibitCreateInstance();
	TacticModeSystem::SingleInstance().ProhibitCreateInstance();

	MeleeEnemyPool::SingleInstance().ClearPool();
	RangedEnemyPool::SingleInstance().ClearPool();
	AutoAttackProjectilePool::SingleInstance().ClearPool();
	EnemyAutoAttackProjectilePool::SingleInstance().ClearPool();
	HealerAutoAttackProjectilePool::SingleInstance().ClearPool();
	MagicianAutoAttackProjectilePool::SingleInstance().ClearPool();
	StatusTimerPool::SingleInstance().ClearPool();
	DebuggingMeshPool::SingleInstance().ClearPool();
	PassiveCakePool::SingleInstance().ClearPool();

	PlayerController::SingleInstance().InitInstance();
	MeleeEnemyPool::SingleInstance().InitInstance();
	RangedEnemyPool::SingleInstance().InitInstance();
	AutoAttackProjectilePool::SingleInstance().InitInstance();
	EnemyAutoAttackProjectilePool::SingleInstance().InitInstance();
	HealerAutoAttackProjectilePool::SingleInstance().InitInstance();
	MagicianAutoAttackProjectilePool::SingleInstance().InitInstance();
	StatusTimerPool::SingleInstance().InitInstance();
	DebuggingMeshPool::SingleInstance().InitInstance();
	PassiveCakePool::SingleInstance().InitInstance();
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
	PassiveCakePool::SingleInstance().PermitCreateInstance();
	MeleeEnemyPool::SingleInstance().PermitCreateInstance();
	RangedEnemyPool::SingleInstance().PermitCreateInstance();
}
