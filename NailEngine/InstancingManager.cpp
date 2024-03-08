#include "InstancingManager.h"

#include "Material.h"
#include "Mesh.h"

#include "CameraManager.h"
#include "NailCamera.h"
#include "NailAnimator.h"
#include "AnimationGroup.h"
#include "NailEngine.h"
#include "ConstantBuffer.h"
#include "ResourceManager.h"
#include "PixelShader.h"


LazyObjects<InstancingManager> InstancingManager::Instance;

void InstancingManager::Init()
{
	instanceTransitionDesc = std::make_shared<InstanceTransitionDesc>();
}

void InstancingManager::RenderStaticDeferred()
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

				auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				auto aabb = i->mesh->GetBoundingBox(i->wtm * CameraManager::Instance.Get().GetMainCamera()->GetVTM(), i->materialIndex);

				if (frustum.Contains(aabb) == DirectX::ContainmentType::DISJOINT)
				{
					continue;
				}

				const std::shared_ptr<RenderInfo>& renderInfo = i;
				InstancingData data;
				data.wtm = renderInfo->wtm;
				AddData(instanceID, data);
			}

			if (renderInfoVec.size() != 0)
			{
				auto& buffer = _buffers[instanceID];
				if (buffer->GetCount() > 0)
				{
					(*renderInfoVec.begin())->material->PushGraphicsData();
					buffer->PushData();
					(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, buffer);
				}
			}
		}
	}
}

void InstancingManager::RenderStaticForward()
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

				auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				auto aabb = i->mesh->GetBoundingBox(i->wtm * CameraManager::Instance.Get().GetMainCamera()->GetVTM(), i->materialIndex);

				if (frustum.Contains(aabb) == DirectX::ContainmentType::DISJOINT)
				{
					continue;
				}

				const std::shared_ptr<RenderInfo>& renderInfo = i;
				InstancingData data;
				data.wtm = renderInfo->wtm;
				AddData(instanceID, data);
			}

			if (renderInfoVec.size() != 0)
			{
				auto& buffer = _buffers[instanceID];
				if (buffer->GetCount() > 0)
				{
					(*renderInfoVec.begin())->material->PushGraphicsData();
					auto test = (*renderInfoVec.begin())->mesh->GetMaterialCount();
					buffer->PushData();
					(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, buffer);
				}
			}
		}
	}
}

void InstancingManager::RenderStaticShadow()
{
	ClearData();

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

				//auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				//auto aabb = i->mesh->GetBoundingBox(i->wtm * CameraManager::Instance.Get().GetMainCamera()->GetVTM(), i->materialIndex);

				//if (frustum.Contains(aabb) == DirectX::ContainmentType::DISJOINT)
				//{
				//	continue;
				//}

				const std::shared_ptr<RenderInfo>& renderInfo = i;
				InstancingData data;
				data.wtm = renderInfo->wtm;
				AddData(instanceID, data);
			}

			if (renderInfoVec.size() != 0)
			{
				auto& buffer = _buffers[instanceID];
				(*renderInfoVec.begin())->shadowMaterial->PushGraphicsData();
				buffer->PushData();
				(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, buffer);
			}
		}
	}
}

void InstancingManager::RenderStaticPointLightShadow()
{
	ClearData();

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

				//auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				//auto aabb = i->mesh->GetBoundingBox(i->wtm * CameraManager::Instance.Get().GetMainCamera()->GetVTM(), i->materialIndex);

				//if (frustum.Contains(aabb) == DirectX::ContainmentType::DISJOINT)
				//{
				//	continue;
				//}

				const std::shared_ptr<RenderInfo>& renderInfo = i;
				InstancingData data;
				data.wtm = renderInfo->wtm;
				AddData(instanceID, data);
			}

			if (renderInfoVec.size() != 0)
			{
				auto& buffer = _buffers[instanceID];
				buffer->PushData();
				(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, buffer);
			}
		}
	}
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

void InstancingManager::RegisterSkinnedData(std::shared_ptr<SkinnedRenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->renderInfo.mesh, (unsigned __int64)renderInfo->renderInfo.material);

	this->skinnedMeshCache[instanceID].insert(renderInfo);

	if (_buffers.find(instanceID) == _buffers.end())
	{
		_buffers[instanceID] = std::make_shared<InstanceBuffer>();
	}
}

void InstancingManager::PopStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);

	auto iter = this->staticMeshDeferredCache.find(instanceID);
	if (iter != this->staticMeshDeferredCache.end())
	{
		this->staticMeshDeferredCache[instanceID].erase(renderInfo);
		if (this->staticMeshDeferredCache[instanceID].empty())
		{
			this->staticMeshDeferredCache.erase(instanceID);
		}
	}
}

void InstancingManager::PopStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);

	auto iter = this->staticMeshForwardCache.find(instanceID);
	if (iter != this->staticMeshForwardCache.end())
	{
		this->staticMeshForwardCache[instanceID].erase(renderInfo);
		if (this->staticMeshForwardCache[instanceID].empty())
		{
			this->staticMeshForwardCache.erase(instanceID);
		}
	}
}

void InstancingManager::PopSkinnedData(std::shared_ptr<SkinnedRenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->renderInfo.mesh, (unsigned __int64)renderInfo->renderInfo.material);

	auto iter = this->skinnedMeshCache.find(instanceID);
	if (iter != this->skinnedMeshCache.end())
	{
		this->skinnedMeshCache[instanceID].erase(renderInfo);
		if (this->skinnedMeshCache[instanceID].empty())
		{
			this->skinnedMeshCache.erase(instanceID);
		}
	}
}

void InstancingManager::RenderSkinned()
{
	ClearData();

	for (auto& pair : this->skinnedMeshCache)
	{
		const std::set<std::shared_ptr<SkinnedRenderInfo>>& renderInfoVec = pair.second;

		const InstanceID instanceID = pair.first;

		{
			int descIndex = 0;
			for (auto& i : renderInfoVec)
			{
				if (i->renderInfo.isActive == false) continue;

				auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				auto aabb = i->renderInfo.mesh->GetBoundingBox(i->renderInfo.wtm * CameraManager::Instance.Get().GetMainCamera()->GetVTM(), i->renderInfo.materialIndex);

				if (frustum.Contains(aabb) == DirectX::ContainmentType::DISJOINT)
				{
					continue;
				}

				const RenderInfo& renderInfo = i->renderInfo;
				InstancingData data;
				data.wtm = renderInfo.wtm;
				AddData(instanceID, data);
				this->instanceTransitionDesc->transitionDesc[descIndex++] = i->animator->GetTransitionDesc();
			}

			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::INST_TRANSITION))->PushGraphicsData(this->instanceTransitionDesc.get(),
				sizeof(InstanceTransitionDesc), static_cast<int>(CB_TYPE::INST_TRANSITION));

			auto animationGroup = ResourceManager::Instance.Get().GetAnimationGroup((*renderInfoVec.begin())->modelName);
			animationGroup->Bind();

			if (renderInfoVec.size() != 0)
			{
				auto& buffer = _buffers[instanceID];
				if (buffer->GetCount() > 0)
				{
					(*renderInfoVec.begin())->renderInfo.material->PushGraphicsData();
					buffer->PushData();
					(*renderInfoVec.begin())->renderInfo.mesh->Render((*renderInfoVec.begin())->renderInfo.materialIndex, buffer);
				}
			}
		}
	}
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

