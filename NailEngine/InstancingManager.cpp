#include "InstancingManager.h"

#include "Material.h"
#include "Mesh.h"

#include "NailCamera.h"
#include "NailAnimator.h"
#include "AnimationGroup.h"
#include "NailEngine.h"
#include "ConstantBuffer.h"
#include "ResourceManager.h"


LazyObjects<InstancingManager> InstancingManager::Instance;

void InstancingManager::Init()
{
	instanceTransitionDesc = std::make_shared<InstanceTransitionDesc>();
}

void InstancingManager::RegisterStaticMeshAndMaterialInDeferred()
{
	// 비활성화인 얘들 거르고
	// 프러스텀 밖에 있는 얘들 거르고
	// 삭제된 얘들 거르고

	// 인스턴스 버퍼의 데이터를 지움
	ClearData();

	//std::map<InstanceID, std::vector<std::shared_ptr<RenderInfo>>> cache;

	//for (auto& each : renderInfo)
	//{
	//	InstanceID instanceID = std::make_pair((unsigned __int64)each->mesh, (unsigned __int64)each->material);

	//	cache[instanceID].push_back(each);
	//}

	for (auto& pair : this->staticMeshDeferredCache)
	{
		std::set<std::shared_ptr<RenderInfo>>& renderInfoVec = pair.second;

		const InstanceID& instanceID = pair.first;

		//if (renderInfoVec.size() == 1)
		//{
		//	MatrixBuffer matrixBuffer;
		//	matrixBuffer.WTM = renderInfoVec[0].wtm;
		//	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
		//	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
		//	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
		//	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
		//	NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
		//
		//	auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(renderInfoVec[0].mesh->GetName()));
		//
		//	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(renderInfoVec[0].material->GetName()))->PushGraphicsData();
		//	for (int i = 0; i < mesh->GetMaterialCount(); ++i)
		//	{
		//		renderInfoVec[0].mesh->Render(i);
		//	}
		//}
		//else
		{
			//for (int i = 0; i < renderInfoVec.size(); ++i)
			for (auto& i : renderInfoVec)
			{
				if (i->isActive == false) continue;

				const std::shared_ptr<RenderInfo>& renderInfo = i;
				InstancingData data;
				data.wtm = renderInfo->wtm;
				AddData(instanceID, data);
			}

			if (renderInfoVec.size() != 0)
			{
				auto& buffer = _buffers[instanceID];
				(*renderInfoVec.begin())->material->PushGraphicsData();
				auto test = (*renderInfoVec.begin())->mesh->GetMaterialCount();
				buffer->PushData();
				(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, buffer);
			}
		}
	}
}

void InstancingManager::RegisterStaticMeshAndMaterialInForward()
{

	// 비활성화인 얘들 거르고
	// 프러스텀 밖에 있는 얘들 거르고
	// 삭제된 얘들 거르고

	// 인스턴스 버퍼의 데이터를 지움
	ClearData();

	//std::map<InstanceID, std::vector<std::shared_ptr<RenderInfo>>> cache;

	//for (auto& each : renderInfo)
	//{
	//	InstanceID instanceID = std::make_pair((unsigned __int64)each->mesh, (unsigned __int64)each->material);

	//	cache[instanceID].push_back(each);
	//}

	for (auto& pair : this->staticMeshForwardCache)
	{
		std::set<std::shared_ptr<RenderInfo>>& renderInfoVec = pair.second;

		const InstanceID& instanceID = pair.first;

		//if (renderInfoVec.size() == 1)
		//{
		//	MatrixBuffer matrixBuffer;
		//	matrixBuffer.WTM = renderInfoVec[0].wtm;
		//	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
		//	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
		//	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
		//	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
		//	NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
		//
		//	auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(renderInfoVec[0].mesh->GetName()));
		//
		//	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(renderInfoVec[0].material->GetName()))->PushGraphicsData();
		//	for (int i = 0; i < mesh->GetMaterialCount(); ++i)
		//	{
		//		renderInfoVec[0].mesh->Render(i);
		//	}
		//}
		//else
		{
			//for (int i = 0; i < renderInfoVec.size(); ++i)
			for (auto& i : renderInfoVec)
			{
				if (i->isActive == false) continue;

				const std::shared_ptr<RenderInfo>& renderInfo = i;
				InstancingData data;
				data.wtm = renderInfo->wtm;
				AddData(instanceID, data);
			}

			if (renderInfoVec.size() != 0)
			{
				auto& buffer = _buffers[instanceID];
				(*renderInfoVec.begin())->material->PushGraphicsData();
				auto test = (*renderInfoVec.begin())->mesh->GetMaterialCount();
				buffer->PushData();
				(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, buffer);
			}
		}
	}
}

void InstancingManager::RegisterMeshAndShadowMaterial(std::vector<RenderInfo>& renderInfo)
{
	//if (renderInfo.size() == 0) return;

	//ClearData();

	//std::map<InstanceID, std::vector<RenderInfo>> cache;

	//for (auto& each : renderInfo)
	//{
	//	InstanceID instanceID = std::make_pair((unsigned __int64)each.mesh, (unsigned __int64)each.material);

	//	cache[instanceID].push_back(each);
	//}

	//for (auto& pair : cache)
	//{
	//	std::vector<RenderInfo>& renderInfoVec = pair.second;

	//	const InstanceID instanceID = pair.first;

	//	//if (renderInfoVec.size() == 1)
	//	//{
	//	//	MatrixBuffer matrixBuffer;
	//	//	matrixBuffer.WTM = renderInfoVec[0].wtm;
	//	//	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	//	//	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	//	//	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	//	//	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//	//	NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
	//	//
	//	//	auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(renderInfoVec[0].mesh->GetName()));
	//	//
	//	//	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(renderInfoVec[0].material->GetName()))->PushGraphicsData();
	//	//	for (int i = 0; i < mesh->GetMaterialCount(); ++i)
	//	//	{
	//	//		renderInfoVec[0].mesh->Render(i);
	//	//	}
	//	//}
	//	//else
	//	{
	//		for (int i = 0; i < renderInfoVec.size(); ++i)
	//		{
	//			RenderInfo& renderInfo = renderInfoVec[i];
	//			InstancingData data;
	//			data.wtm = renderInfo.wtm;
	//			AddData(instanceID, data);
	//		}

	//		auto& buffer = _buffers[instanceID];
	//		renderInfoVec[0].shadowMaterial.PushGraphicsData();
	//		auto test = renderInfoVec[0].mesh->GetMaterialCount();
	//		buffer->PushData();
	//		renderInfoVec[0].mesh->Render(renderInfoVec[0].materialIndex, buffer);
	//	}
	//}
}

void InstancingManager::RegisterStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);

	this->staticMeshDeferredCache[instanceID].insert(renderInfo);

	if (_buffers.find(instanceID) == _buffers.end())
	{
		_buffers[instanceID] = std::make_shared<InstanceBuffer>();
	}
}

void InstancingManager::RegisterStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);

	this->staticMeshForwardCache[instanceID].insert(renderInfo);

	if (_buffers.find(instanceID) == _buffers.end())
	{
		_buffers[instanceID] = std::make_shared<InstanceBuffer>();
	}
}

void InstancingManager::PopStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);

	this->staticMeshDeferredCache[instanceID].erase(renderInfo);
}

void InstancingManager::PopStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);

	this->staticMeshForwardCache[instanceID].erase(renderInfo);
}

void InstancingManager::RegisterSkinnedMeshAndMaterial(std::set<std::shared_ptr<SkinnedRenderInfo>>& renderInfo)
{
	/*if (renderInfo.size() == 0) return;

	ClearData();

	std::map<InstanceID, std::vector<SkinnedRenderInfo>> cache;

	for (auto& each : renderInfo)
	{
		InstanceID instanceID = std::make_pair((unsigned __int64)each.renderInfo.mesh, (unsigned __int64)each.renderInfo.material);

		cache[instanceID].push_back(each);
	}

	for (auto& pair : cache)
	{
		const std::vector<SkinnedRenderInfo>& renderInfoVec = pair.second;

		const InstanceID instanceID = pair.first;

		{
			for (int i = 0; i < renderInfoVec.size(); ++i)
			{
				const RenderInfo& renderInfo = renderInfoVec[i].renderInfo;
				InstancingData data;
				data.wtm = renderInfo.wtm;
				AddData(instanceID, data);
				this->instanceTransitionDesc->transitionDesc[i] = renderInfoVec[i].animator->GetTransitionDesc();
			}

			NailEngine::Instance.Get().GetConstantBuffer(5)->PushGraphicsData(this->instanceTransitionDesc.get(),
				sizeof(InstanceTransitionDesc), 5);

			auto animationGroup = ResourceManager::Instance.Get().GetAnimationGroup(renderInfoVec[0].modelName);
			animationGroup->Bind();

			auto& buffer = _buffers[instanceID];
			renderInfoVec[0].renderInfo.material->PushGraphicsData();
			buffer->PushData();
			renderInfoVec[0].renderInfo.mesh->Render(renderInfoVec[0].renderInfo.materialIndex, buffer);
		}
	}*/
}

void InstancingManager::ClearData()
{
	for (auto& pair : _buffers)
	{
		pair.second->ClearData();
	}
}


void InstancingManager::AddData(const InstanceID& id, InstancingData& instancingData)
{
	_buffers[id]->AddData(instancingData);
}

