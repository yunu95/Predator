#pragma once
#include "Utils.h"


#include "RenderSystem.h"

#include <memory>
#include <mutex>
#include <thread>
#include <map>


class FrustumCullingManager
{
public:
	static LazyObjects<FrustumCullingManager> Instance;
	friend LazyObjects<FrustumCullingManager>;

public:
	void Init();
	void Wait();

private:
	void FrustumCulling();

public:
	void SetCameraChange(bool isChange) {this->isChange = isChange;	};

#pragma region Getter
	std::shared_ptr<std::mutex> GetRenderInfoMutex(std::shared_ptr<RenderInfo>& renderInfo);

#pragma endregion


#pragma region Register
	void RegisterRenderInfo(const std::shared_ptr<RenderInfo>& renderInfo);

#pragma endregion

private:
	bool isChange = false;
	std::thread thread;
	std::map<std::shared_ptr<RenderInfo>, std::shared_ptr<std::mutex>> renderInfoMutexMap;

};

