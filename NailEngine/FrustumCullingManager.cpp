#include "FrustumCullingManager.h"
#include "CameraManager.h"
#include "InstancingManager.h"
#include "NailCamera.h"


LazyObjects<FrustumCullingManager> FrustumCullingManager::Instance;

void FrustumCullingManager::Init()
{
	this->thread = std::thread([&]() {
		this->FrustumCulling();
		});

};

void FrustumCullingManager::Wait()
{
	if (thread.joinable())
	{
		thread.join();
	}
}

void FrustumCullingManager::FrustumCulling()
{
	// 카메라 업데이트가 끝나면

	auto& renderInfoVec = InstancingManager::Instance.Get().GetStaticRenderInfoVec();

	//static bool onceCulled = false;
	//static int counter = 0;
	//counter++;
	//if (onceCulled)
	//	return;
	//if (renderInfoVec.size() > 0 && counter > 300)
	//	onceCulled = true;

	for (auto& each : renderInfoVec)
	{
		for (auto& each2 : each.second)
		{
			auto iter = this->renderInfoMutexMap.find(each2);
			//std::scoped_lock<std::mutex> lock(*iter->second.get());

			// 컬링 로직 실행
			{
				if (iter->first == nullptr) continue;

				if (iter->first->mesh == nullptr) continue;

				if (iter->first->isActive == false) continue;

				auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				auto aabb = iter->first->mesh->GetBoundingBox(iter->first->wtm, iter->first->materialIndex);

				if (frustum.Contains(aabb) == DirectX::ContainmentType::DISJOINT)
				{
					iter->first->isCulled = true;
				}
				else
				{
					iter->first->isCulled = false;
				}
			}

			if (this->isChange)
			{
				break;
			}
		}


		// 카메라가 업데이트되면
		// break;
		if (this->isChange)
		{
			break;
		}
	}
}

std::shared_ptr<std::mutex> FrustumCullingManager::GetRenderInfoMutex(std::shared_ptr<RenderInfo>& renderInfo)
{
	auto iter = this->renderInfoMutexMap.find(renderInfo);
	if (iter != this->renderInfoMutexMap.end())
	{
		return iter->second;
	}

	return nullptr;
}

void FrustumCullingManager::RegisterRenderInfo(const std::shared_ptr<RenderInfo>& renderInfo)
{
	this->renderInfoMutexMap.insert({ renderInfo, std::make_shared<std::mutex>() });
}
