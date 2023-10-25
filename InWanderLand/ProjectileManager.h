#pragma once
#include <vector>
#include <unordered_set>

template <class ProjectileType>
class ProjectilePool;

class ProjectileManager
{
public:
	ProjectileManager() { instance = this; }
	static ProjectileManager* GetInstance() { return instance; }
	//template<class ProjectileType>
	//std::unordered_set<ProjectilePool<ProjectileType>*> projectilePoolList;
	
private:
	static ProjectileManager* instance;
};