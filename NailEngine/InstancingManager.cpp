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
#include "GeometryShader.h"
#include "PointLight.h"
#include "ParticleSystem.h"
#include "FrustumCullingManager.h"

#include <cmath>
#include <algorithm>

LazyObjects<InstancingManager> InstancingManager::Instance;

void InstancingManager::Init()
{
	instanceTransitionDesc = std::make_shared<InstanceTransitionDesc>();
	particleBuffer = std::make_shared<ParticleBuffer>();
	lightMapUVBuffer = std::make_shared<LightMapUVBuffer>();
}

bool InstancingManager::IsInTree(std::shared_ptr<RenderInfo>& renderInfo)
{
	auto iter = this->staticMeshRenderInfoIndexMap.find(renderInfo);
	if (iter != this->staticMeshRenderInfoIndexMap.end())
	{
		return true;
	}

	return false;
}

void InstancingManager::SortByCameraDirection()
{
	if (!this->staticMeshDeferredRenderVec.empty())
	{
		for (auto& each : this->staticMeshDeferredRenderVec)
		{
			each.second.erase(
				std::remove_if(each.second.begin(), each.second.end(),
					[](const std::shared_ptr<RenderInfo>& ptr) {
						return ptr == nullptr;
					}),
				each.second.end());
		}
	}
	//if (!this->staticMeshDeferredRenderVec.empty())
	//{
	//	this->staticMeshDeferredRenderVec.erase(
	//		std::remove_if(this->staticMeshDeferredRenderVec.begin(), this->staticMeshDeferredRenderVec.end(),
	//			[](const std::pair<InstanceID, std::vector<std::shared_ptr<RenderInfo>>>& pair) {
	//				return pair.second.empty();
	//			}),
	//		this->staticMeshDeferredRenderVec.end());
	//}

	// 쿼드트리에 중복으로 데이터가 들어가지 않게 쿼드트리에서 데이터를 빼준다.
	if (!this->staticMeshDeferredRenderVec.empty())
	{
		for (auto& each : this->staticMeshDeferredRenderVec)
		{
			for (auto& each2 : each.second)
			{
				if (each2 == nullptr) continue;

				this->quadTree.Remove(each2.get());
			}
		}
	}

	for (auto& each : this->staticMeshDeferredMap)
	{
		std::vector<std::shared_ptr<RenderInfo>> tempVec;

		for (auto& each2 : each.second)
		{
			InstanceID instanceID = std::make_pair(each2->mesh, each2->material);

			auto instanceIter = this->staticMeshInstanceIDIndexMap.find(instanceID);
			if (instanceIter != this->staticMeshInstanceIDIndexMap.end())
			{
				this->staticMeshDeferredRenderVec[instanceIter->second].second.push_back(each2);
				continue;
			}

			tempVec.push_back(each2);
		}

		if (!tempVec.empty())
		{
			auto tempPair = std::make_pair(each.first, tempVec);
			this->staticMeshDeferredRenderVec.push_back(tempPair);
		}
	}
	staticMeshDeferredMap.clear();


	// 우선 BoundingRadius 기준으로 정렬
	//std::sort(this->staticMeshDeferredRenderVec.begin(), this->staticMeshDeferredRenderVec.end(),
	//	[](const auto& left, const auto& right)
	//	{
	//		auto lR = left.first.first->GetBoundingRadius();
	//		auto rR = right.first.first->GetBoundingRadius();

	//		return lR > rR;
	//	}
	//);

	std::vector<std::pair<InstanceID, std::vector<std::shared_ptr<RenderInfo>>>> noBlend;
	std::vector<std::pair<InstanceID, std::vector<std::shared_ptr<RenderInfo>>>> blend;

	for (auto& each : this->staticMeshDeferredRenderVec)
	{
		D3D11_BLEND_DESC desc;
		((PixelShader*)(each.first.second->GetPixelShader()))->GetBlendState()->GetDesc(&desc);

		if (desc.RenderTarget[0].BlendEnable)
		{
			blend.push_back(each);
		}
		else
		{
			noBlend.push_back(each);
		}
	}


	std::sort(noBlend.begin(), noBlend.end(),
		[](const auto& left, const auto& right)
		{
			auto lR = left.first.first->GetBoundingRadius();
			auto rR = right.first.first->GetBoundingRadius();

			return lR > rR;
		}
	);
	std::sort(blend.begin(), blend.end(),
		[](const auto& left, const auto& right)
		{
			auto lR = left.first.first->GetBoundingRadius();
			auto rR = right.first.first->GetBoundingRadius();

			return lR < rR;
		}
	);

	this->staticMeshDeferredRenderVec.clear();
	for (auto& each : noBlend)
	{
		this->staticMeshDeferredRenderVec.push_back(each);
	}

	for (auto& each : blend)
	{
		this->staticMeshDeferredRenderVec.push_back(each);
	}

	DirectX::SimpleMath::Matrix cameraWTM = CameraManager::Instance.Get().GetMainCamera()->GetWTM();
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	cameraWTM.Decompose(scale, quat, pos);

	DirectX::SimpleMath::Matrix rot = DirectX::XMMatrixRotationQuaternion(quat);

	DirectX::SimpleMath::Vector4 front{ 0.f,0.f,1.f,0.f };
	front = DirectX::XMVector3Rotate(front, quat);


	DirectX::SimpleMath::Vector3 cameraDirection = DirectX::SimpleMath::Vector3{ front.x,front.y,front.z };
	cameraDirection.Normalize();

	for (auto& each : this->staticMeshDeferredRenderVec)
	{
		std::sort(each.second.begin(), each.second.end(),
			[=](const auto& left, const auto& right)
			{
				if (right == nullptr || left == nullptr)
					return false;

				auto lPos = DirectX::SimpleMath::Vector3{ left->wtm._41, left->wtm._42, left->wtm._43 };
				auto rPos = DirectX::SimpleMath::Vector3{ right->wtm._41, right->wtm._42, right->wtm._43 };

				float lDot = cameraDirection.Dot(lPos);
				float rDot = cameraDirection.Dot(rPos);

				return lDot < rDot;
			}
		);
	}

	// 인덱스맵에 맵핑하기
	unsigned int instanceIDIndex = 0;
	unsigned int renderInfoIndex = 0;
	staticMeshInstanceIDIndexMap.clear();
	staticMeshRenderInfoIndexMap.clear();
	for (auto& each : staticMeshDeferredRenderVec)
	{
		staticMeshInstanceIDIndexMap.insert({ each.first,instanceIDIndex });
		instanceIDIndex++;

		renderInfoIndex = 0;

		for (auto& each2 : each.second)
		{
			staticMeshRenderInfoIndexMap.insert({ each2 ,renderInfoIndex });
			renderInfoIndex++;
		}
	}

	// 쿼드트리에 RenderInfo 등록
	for (auto& each : this->staticMeshDeferredRenderVec)
	{
		for (auto& each2 : each.second)
		{
			if (each2 == nullptr) continue;

			DirectX::SimpleMath::Vector3 tempScale;
			DirectX::SimpleMath::Vector3 tempPos;
			DirectX::SimpleMath::Quaternion tempQuat;
			each2->wtm.Decompose(tempScale, tempQuat, tempPos);
			// 바운딩 볼륨 반지름 조정
			auto radius = each.first.first->GetBoundingRadius();
			radius = radius * std::max(std::max(tempScale.x, tempScale.y), tempScale.z);
			this->quadTree.PushData(each2.get(), DirectX::SimpleMath::Vector2{ tempPos.x, tempPos.z }, radius);
		}
	}
}

void InstancingManager::RenderStaticDeferred()
{
	// 비활성화인 얘들 거르고
	// 프러스텀 밖에 있는 얘들 거르고
	// 삭제된 얘들 거르고

	// 인스턴스 버퍼의 데이터를 지움
	ClearData();

	// 컬링과 Render하기 전에 쿼드트리로부터 쿼리를 함
	DirectX::SimpleMath::Vector2 minPoint;
	DirectX::SimpleMath::Vector2 maxPoint;
	auto camera = CameraManager::Instance.Get().GetMainCamera();
	camera->GetCameraAreaXZ(minPoint, maxPoint);
	auto queryVec = this->quadTree.QueryData(minPoint, maxPoint);

	auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
	for (auto& each : queryVec)
	{
		each->isInArea = true;

		auto aabb = each->mesh->GetBoundingBox(each->wtm, each->materialIndex);

		if (frustum.Contains(aabb) == DirectX::ContainmentType::DISJOINT)
		{
			each->isCulled = true;
		}
	}

	{
		for (auto& pair : this->staticMeshDeferredMap)
		{
			auto& renderInfoVec = pair.second;

			const InstanceID& instanceID = pair.first;

			{
				//for (int i = 0; i < renderInfoVec.size(); ++i)
				int index = 0;
				for (auto& i : renderInfoVec)
				{
					if (i->mesh == nullptr) continue;

					if (i->isActive == false) continue;

					const std::shared_ptr<RenderInfo>& renderInfo = i;
					InstancingData data;
					data.wtm = renderInfo->wtm;
					AddData(instanceID, data);

					lightMapUVBuffer->lightMapUV[index].lightMapIndex = renderInfo->lightMapIndex;
					lightMapUVBuffer->lightMapUV[index].scaling = renderInfo->uvScaling;
					lightMapUVBuffer->lightMapUV[index].uvOffset = renderInfo->uvOffset;

					index++;
				}

				NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::LIGHTMAP_UV))->PushGraphicsData(lightMapUVBuffer.get(),
					sizeof(LightMapUVBuffer),
					static_cast<int>(CB_TYPE::LIGHTMAP_UV), false);

				if (renderInfoVec.size() != 0)
				{
					auto& buffer = _buffers[instanceID];
					if (buffer->GetCount() > 0)
					{
						ExposureBuffer exposurrBuffer;
						exposurrBuffer.diffuseExposure = (*renderInfoVec.begin())->mesh->GetDiffuseExposure();
						exposurrBuffer.ambientExposure = (*renderInfoVec.begin())->mesh->GetAmbientExposure();;
						NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::EXPOSURE))->PushGraphicsData(&exposurrBuffer,
							sizeof(ExposureBuffer),
							static_cast<int>(CB_TYPE::EXPOSURE), false);

						(*renderInfoVec.begin())->material->PushGraphicsData();
						buffer->PushData();
						(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
					}
				}
			}
		}
	}

	{
		for (auto& pair : this->staticMeshDeferredRenderVec)
		{
			auto& renderInfoVec = pair.second;

			const InstanceID& instanceID = pair.first;

			{
				int index = 0;
				for (auto& i : renderInfoVec)
				{
					if (i == nullptr) continue;

					if (i.get() == nullptr)
					{
						continue;
					}

					if (i.use_count() == 0)
					{
						continue;
					}

					if (i->mesh == nullptr) continue;

					if (i->isInArea == false)
					{
						continue;
					}

					if (i->isActive == false) continue;

					if (i->isCulled == true)
					{
						i->isCulled = false;
						continue;
					}

					const std::shared_ptr<RenderInfo>& renderInfo = i;
					InstancingData data;
					data.wtm = renderInfo->wtm;
					AddData(instanceID, data);

					lightMapUVBuffer->lightMapUV[index].lightMapIndex = renderInfo->lightMapIndex;
					lightMapUVBuffer->lightMapUV[index].scaling = renderInfo->uvScaling;
					lightMapUVBuffer->lightMapUV[index].uvOffset = renderInfo->uvOffset;

					index++;

					i->isInArea = false;
					i->isCulled = false;
				}

				NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::LIGHTMAP_UV))->PushGraphicsData(lightMapUVBuffer.get(),
					sizeof(LightMapUVBuffer),
					static_cast<int>(CB_TYPE::LIGHTMAP_UV), false);

				if (renderInfoVec.size() != 0)
				{
					auto& buffer = _buffers[instanceID];
					if (buffer->GetCount() > 0)
					{
						std::shared_ptr<RenderInfo> renderInfo = nullptr;
						for (auto& each : renderInfoVec)
						{
							if (each.get())
							{
								renderInfo = each;
							}
						}

						if (renderInfo == nullptr)
						{
							return;
						}

						ExposureBuffer exposurrBuffer;
						exposurrBuffer.diffuseExposure = renderInfo->mesh->GetDiffuseExposure();
						exposurrBuffer.ambientExposure = renderInfo->mesh->GetAmbientExposure();
						NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::EXPOSURE))->PushGraphicsData(&exposurrBuffer,
							sizeof(ExposureBuffer),
							static_cast<int>(CB_TYPE::EXPOSURE), false);

						renderInfo->material->PushGraphicsData();
						buffer->PushData();
						renderInfo->mesh->Render(renderInfo->materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
					}
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

		{
			for (auto& i : renderInfoVec)
			{
				if (i->mesh == nullptr) continue;
				if (i->isActive == false) continue;

				auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				auto aabb = i->mesh->GetBoundingBox(i->wtm, i->materialIndex);

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
				if ((*renderInfoVec.begin())->mesh == nullptr) continue;
				auto& buffer = _buffers[instanceID];
				if (buffer->GetCount() > 0)
				{
					(*renderInfoVec.begin())->material->PushGraphicsData();
					auto test = (*renderInfoVec.begin())->mesh->GetMaterialCount();
					buffer->PushData();
					(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
				}
			}
		}
	}
}

void InstancingManager::RenderStaticShadow()
{
	ClearData();

	for (auto& pair : this->staticMeshDeferredMap)
	{
		auto& renderInfoVec = pair.second;

		const InstanceID& instanceID = pair.first;

		{
			for (auto& i : renderInfoVec)
			{
				if (i->lightMapIndex != -1) continue;

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
				if (buffer->GetCount() > 0)
				{
					auto opacityMap = (*renderInfoVec.begin())->material->GetTexture(yunuGI::Texture_Type::OPACITY);
					if (opacityMap)
					{
						static_cast<Texture*>(opacityMap)->Bind(static_cast<unsigned int>(yunuGI::Texture_Type::OPACITY));
						MaterialBuffer materialBuffer;
						materialBuffer.useTexture[static_cast<unsigned int>(yunuGI::Texture_Type::OPACITY)] = 1;
						NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATERIAL))->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer), static_cast<int>(CB_TYPE::MATERIAL));
					}
					else
					{
						MaterialBuffer materialBuffer;
						materialBuffer.useTexture[static_cast<unsigned int>(yunuGI::Texture_Type::OPACITY)] = 0;
						NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATERIAL))->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer), static_cast<int>(CB_TYPE::MATERIAL));
					}

					buffer->PushData();
					(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
				}
			}
		}
	}

	for (auto& pair : this->staticMeshDeferredRenderVec)
	{
		auto& renderInfoVec = pair.second;

		const InstanceID& instanceID = pair.first;

		{
			for (auto& i : renderInfoVec)
			{
				if (i == nullptr) continue;

				if (i->lightMapIndex != -1) continue;

				if (i->isActive == false) continue;

				const std::shared_ptr<RenderInfo>& renderInfo = i;
				InstancingData data;
				data.wtm = renderInfo->wtm;
				AddData(instanceID, data);
			}

			if (renderInfoVec.size() != 0)
			{
				if (renderInfoVec[0] == nullptr) continue;

				auto& buffer = _buffers[instanceID];
				if (buffer->GetCount() > 0)
				{
					auto opacityMap = (*renderInfoVec.begin())->material->GetTexture(yunuGI::Texture_Type::OPACITY);
					if (opacityMap)
					{
						static_cast<Texture*>(opacityMap)->Bind(static_cast<unsigned int>(yunuGI::Texture_Type::OPACITY));
						MaterialBuffer materialBuffer;
						materialBuffer.useTexture[static_cast<unsigned int>(yunuGI::Texture_Type::OPACITY)] = 1;
						NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATERIAL))->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer), static_cast<int>(CB_TYPE::MATERIAL));
					}
					else
					{
						MaterialBuffer materialBuffer;
						materialBuffer.useTexture[static_cast<unsigned int>(yunuGI::Texture_Type::OPACITY)] = 0;
						NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATERIAL))->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer), static_cast<int>(CB_TYPE::MATERIAL));
					}

					buffer->PushData();
					(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
				}
			}
		}
	}
}

void InstancingManager::RenderSkinnedShadow()
{
	ClearData();

	for (auto& pair : this->skinnedMeshDeferredCache)
	{
		const std::set<std::shared_ptr<SkinnedRenderInfo>>& renderInfoVec = pair.second;

		const InstanceID instanceID = pair.first;

		{
			int descIndex = 0;
			int index = 0;
			for (auto& i : renderInfoVec)
			{
				if (i->renderInfo.isActive == false) continue;

				auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				auto aabb = i->renderInfo.mesh->GetBoundingBox(i->renderInfo.wtm, i->renderInfo.materialIndex);

				if (frustum.Intersects(aabb) == false)
				{
					continue;
				}

				const RenderInfo& renderInfo = i->renderInfo;
				InstancingData data;
				data.wtm = renderInfo.wtm;
				AddData(instanceID, data);
				this->instanceTransitionDesc->transitionDesc[descIndex++] = i->animator->GetTransitionDesc();

				lightMapUVBuffer->lightMapUV[index].lightMapIndex = renderInfo.lightMapIndex;
				lightMapUVBuffer->lightMapUV[index].scaling = renderInfo.uvScaling;
				lightMapUVBuffer->lightMapUV[index].uvOffset = renderInfo.uvOffset;

				index++;
			}

			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::LIGHTMAP_UV))->PushGraphicsData(lightMapUVBuffer.get(),
				sizeof(LightMapUVBuffer),
				static_cast<int>(CB_TYPE::LIGHTMAP_UV), false);

			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::INST_TRANSITION))->PushGraphicsData(this->instanceTransitionDesc.get(),
				sizeof(InstanceTransitionDesc), static_cast<int>(CB_TYPE::INST_TRANSITION));

			auto animationGroup = ResourceManager::Instance.Get().GetAnimationGroup((*renderInfoVec.begin())->modelName);
			animationGroup->Bind();

			if (renderInfoVec.size() != 0)
			{
				if ((*renderInfoVec.begin())->renderInfo.mesh == nullptr) continue;

				//ExposureBuffer exposurrBuffer;
				//exposurrBuffer.diffuseExposure = (*renderInfoVec.begin())->renderInfo.mesh->GetDiffuseExposure();
				//exposurrBuffer.ambientExposure = (*renderInfoVec.begin())->renderInfo.mesh->GetAmbientExposure();;
				//NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::EXPOSURE))->PushGraphicsData(&exposurrBuffer,
				//	sizeof(ExposureBuffer),
				//	static_cast<int>(CB_TYPE::EXPOSURE), false);

				auto& buffer = _buffers[instanceID];

				if (buffer->GetCount() > 0)
				{
					auto opacityMap = (*renderInfoVec.begin())->renderInfo.material->GetTexture(yunuGI::Texture_Type::OPACITY);
					if (opacityMap)
					{
						static_cast<Texture*>(opacityMap)->Bind(static_cast<unsigned int>(yunuGI::Texture_Type::OPACITY));
						MaterialBuffer materialBuffer;
						materialBuffer.useTexture[static_cast<unsigned int>(yunuGI::Texture_Type::OPACITY)] = 1;
						NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATERIAL))->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer), static_cast<int>(CB_TYPE::MATERIAL));
					}
					else
					{
						MaterialBuffer materialBuffer;
						materialBuffer.useTexture[static_cast<unsigned int>(yunuGI::Texture_Type::OPACITY)] = 0;
						NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATERIAL))->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer), static_cast<int>(CB_TYPE::MATERIAL));
					}

					buffer->PushData();
					(*renderInfoVec.begin())->renderInfo.mesh->Render((*renderInfoVec.begin())->renderInfo.materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
				}
			}
		}
	}

}

void InstancingManager::RenderStaticPointLightShadow(DirectX::SimpleMath::Matrix& lightWTM, PointLight* light)
{
	ClearData();

	for (auto& pair : this->staticMeshDeferredMap)
	{
		auto& renderInfoVec = pair.second;

		const InstanceID& instanceID = pair.first;

		{
			for (auto& i : renderInfoVec)
			{
				if (i->lightMapIndex != -1) continue;

				if (i->isActive == false) continue;

				// 만일 포인트라이트의 범위 안에 있는 오브젝트가 아니라면 렌더링되지 않게 컬링
				auto aabb = i->mesh->GetBoundingBox(i->wtm, i->materialIndex);

				if (light->GetBoundingSphere(lightWTM).Intersects(aabb) == false)
				{
					continue;
				}

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
				(*renderInfoVec.begin())->mesh->Render((*renderInfoVec.begin())->materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
			}
		}
	}
}

void InstancingManager::RenderSkinnedPointLightShadow(DirectX::SimpleMath::Matrix& lightWTM, PointLight* light)
{
	ClearData();

	for (auto& pair : this->skinnedMeshDeferredCache)
	{
		const std::set<std::shared_ptr<SkinnedRenderInfo>>& renderInfoVec = pair.second;

		const InstanceID instanceID = pair.first;

		{
			int descIndex = 0;
			for (auto& i : renderInfoVec)
			{
				if (i->renderInfo.isActive == false) continue;

				auto aabb = i->renderInfo.mesh->GetBoundingBox(i->renderInfo.wtm, i->renderInfo.materialIndex);

				if (light->GetBoundingSphere(lightWTM).Intersects(aabb) == false)
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
					buffer->PushData();
					(*renderInfoVec.begin())->renderInfo.mesh->Render((*renderInfoVec.begin())->renderInfo.materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
				}
			}
		}
	}
}

void InstancingManager::RegisterStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo)
{
	//InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);
	InstanceID instanceID = std::make_pair(renderInfo->mesh, renderInfo->material);

	auto renderInfoIter = this->staticMeshRenderInfoIndexMap.find(renderInfo);
	if (renderInfoIter != this->staticMeshRenderInfoIndexMap.end())
	{
		// 이곳에 들어오면 renderInfo포인터가 vector에 있었다는 뜻
		auto instanceIter = this->staticMeshInstanceIDIndexMap.find(instanceID);
		if (instanceIter != this->staticMeshInstanceIDIndexMap.end())
		{
			// instancID가 이미 그룹내에 있다면 동일한 그룹에 넣어줌
			this->staticMeshDeferredRenderVec[instanceIter->second].second.push_back(renderInfo);
		}
		else
		{
			// 원래 vector안에는 있었지만 새로운 머터리얼이 적용되어 정보가 없는 경우
			std::vector<std::shared_ptr<RenderInfo>> tempVec;
			tempVec.push_back(renderInfo);
			this->staticMeshDeferredRenderVec.push_back(std::make_pair(instanceID, tempVec));

			// 인덱스맵도 맵핑하는 코드 추가하기
			this->staticMeshInstanceIDIndexMap.insert({ instanceID, staticMeshInstanceIDIndexMap.size() });
			this->staticMeshRenderInfoIndexMap.insert({ renderInfo,tempVec.size() - 1 });
		}

		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion quat;
		renderInfo->wtm.Decompose(scale, quat, pos);

		float radius = std::max(std::max(scale.x, scale.y), scale.z);

		radius *= renderInfo->mesh->GetBoundingRadius();

		// 나중에는 RenderInfo에 radius를 추가해서 Decompose하는것을 빼자.
		this->quadTree.PushData(renderInfo.get(), DirectX::SimpleMath::Vector2{ renderInfo->wtm._41,renderInfo->wtm._43 },
			radius);
	}
	else
	{
		this->staticMeshDeferredMap[instanceID].insert(renderInfo);
	}

	if (_buffers.find(instanceID) == _buffers.end())
	{
		_buffers[instanceID] = std::make_shared<InstanceBuffer>();
	}
}

void InstancingManager::RegisterStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo)
{
	//InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);
	InstanceID instanceID = std::make_pair(renderInfo->mesh, renderInfo->material);

	this->staticMeshForwardCache[instanceID].insert(renderInfo);

	if (_buffers.find(instanceID) == _buffers.end())
	{
		_buffers[instanceID] = std::make_shared<InstanceBuffer>();
	}
}

void InstancingManager::RegisterSkinnedDeferredData(std::shared_ptr<SkinnedRenderInfo>& renderInfo)
{
	//InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->renderInfo.mesh, (unsigned __int64)renderInfo->renderInfo.material);
	InstanceID instanceID = std::make_pair(renderInfo->renderInfo.mesh, renderInfo->renderInfo.material);

	this->skinnedMeshDeferredCache[instanceID].insert(renderInfo);

	if (_buffers.find(instanceID) == _buffers.end())
	{
		_buffers[instanceID] = std::make_shared<InstanceBuffer>();
	}
}

void InstancingManager::RegisterSkinnedForwardData(std::shared_ptr<SkinnedRenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair(renderInfo->renderInfo.mesh, renderInfo->renderInfo.material);

	this->skinnedMeshForwardCache[instanceID].insert(renderInfo);

	if (_buffers.find(instanceID) == _buffers.end())
	{
		_buffers[instanceID] = std::make_shared<InstanceBuffer>();
	}
}

void InstancingManager::PopStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo)
{
	//InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);
	InstanceID instanceID = std::make_pair(renderInfo->mesh, renderInfo->material);

	// 인스턴스 인덱스 맵에 있는지 검사
	auto instanceIter = this->staticMeshInstanceIDIndexMap.find(instanceID);
	if (instanceIter != this->staticMeshInstanceIDIndexMap.end())
	{
		// 이미 vector에 있다는 뜻
		auto renderInfoIter = this->staticMeshRenderInfoIndexMap.find(renderInfo);
		if (renderInfoIter != this->staticMeshRenderInfoIndexMap.end())
		{
			if (!this->staticMeshDeferredRenderVec[instanceIter->second].second.empty())
			{
				this->staticMeshDeferredRenderVec[instanceIter->second].second[renderInfoIter->second] = nullptr;
			}


			// 인덱스맵에도 null을 넣어서 맵핑하는 코드 추가
			staticMeshRenderInfoIndexMap.erase(renderInfo);
			staticMeshRenderInfoIndexMap.insert({ nullptr, -1 });

			// 쿼드트리에서 데이터 삭제
			this->quadTree.Remove(renderInfo.get());
		}
	}


	auto iter = this->staticMeshDeferredMap.find(instanceID);

	if (iter != this->staticMeshDeferredMap.end())
	{
		this->staticMeshDeferredMap[instanceID].erase(renderInfo);

		if (this->staticMeshDeferredMap[instanceID].empty())
		{
			this->staticMeshDeferredMap.erase(instanceID);
		}
	}
}

void InstancingManager::PopStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo)
{
	//InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->mesh, (unsigned __int64)renderInfo->material);
	InstanceID instanceID = std::make_pair(renderInfo->mesh, renderInfo->material);

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

void InstancingManager::PopSkinnedDeferredData(std::shared_ptr<SkinnedRenderInfo>& renderInfo)
{
	//InstanceID instanceID = std::make_pair((unsigned __int64)renderInfo->renderInfo.mesh, (unsigned __int64)renderInfo->renderInfo.material);
	InstanceID instanceID = std::make_pair(renderInfo->renderInfo.mesh, renderInfo->renderInfo.material);

	auto iter = this->skinnedMeshDeferredCache.find(instanceID);
	if (iter != this->skinnedMeshDeferredCache.end())
	{
		this->skinnedMeshDeferredCache[instanceID].erase(renderInfo);
		if (this->skinnedMeshDeferredCache[instanceID].empty())
		{
			this->skinnedMeshDeferredCache.erase(instanceID);
		}
	}
}

void InstancingManager::PopSkinnedForwardData(std::shared_ptr<SkinnedRenderInfo>& renderInfo)
{
	InstanceID instanceID = std::make_pair(renderInfo->renderInfo.mesh, renderInfo->renderInfo.material);

	auto iter = this->skinnedMeshForwardCache.find(instanceID);
	if (iter != this->skinnedMeshForwardCache.end())
	{
		this->skinnedMeshForwardCache[instanceID].erase(renderInfo);
		if (this->skinnedMeshForwardCache[instanceID].empty())
		{
			this->skinnedMeshForwardCache.erase(instanceID);
		}
	}
}

void InstancingManager::RegisterParticleRenderInfo(ParticleSystem* particleSystem, std::list<yunuGI::ParticleRenderInfo>* particleInfoList)
{
	this->particleRenderInfoMap.insert({ particleSystem, particleInfoList });
}

void InstancingManager::RenderSkinnedDeferred()
{
	ClearData();

	for (auto& pair : this->skinnedMeshDeferredCache)
	{
		const std::set<std::shared_ptr<SkinnedRenderInfo>>& renderInfoVec = pair.second;

		const InstanceID instanceID = pair.first;

		{
			int descIndex = 0;
			int index = 0;
			for (auto& i : renderInfoVec)
			{
				if (i->renderInfo.isActive == false) continue;

				auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				auto aabb = i->renderInfo.mesh->GetBoundingBox(i->renderInfo.wtm, i->renderInfo.materialIndex);

				if (frustum.Intersects(aabb) == false)
				{
					continue;
				}

				const RenderInfo& renderInfo = i->renderInfo;
				InstancingData data;
				data.wtm = renderInfo.wtm;
				AddData(instanceID, data);
				this->instanceTransitionDesc->transitionDesc[descIndex++] = i->animator->GetTransitionDesc();

				lightMapUVBuffer->lightMapUV[index].lightMapIndex = renderInfo.lightMapIndex;
				lightMapUVBuffer->lightMapUV[index].scaling = renderInfo.uvScaling;
				lightMapUVBuffer->lightMapUV[index].uvOffset = renderInfo.uvOffset;

				index++;
			}

			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::LIGHTMAP_UV))->PushGraphicsData(lightMapUVBuffer.get(),
				sizeof(LightMapUVBuffer),
				static_cast<int>(CB_TYPE::LIGHTMAP_UV), false);

			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::INST_TRANSITION))->PushGraphicsData(this->instanceTransitionDesc.get(),
				sizeof(InstanceTransitionDesc), static_cast<int>(CB_TYPE::INST_TRANSITION));

			auto animationGroup = ResourceManager::Instance.Get().GetAnimationGroup((*renderInfoVec.begin())->modelName);
			animationGroup->Bind();

			if (renderInfoVec.size() != 0)
			{
				if ((*renderInfoVec.begin())->renderInfo.mesh == nullptr) continue;

				ExposureBuffer exposurrBuffer;
				exposurrBuffer.diffuseExposure = (*renderInfoVec.begin())->renderInfo.mesh->GetDiffuseExposure();
				exposurrBuffer.ambientExposure = (*renderInfoVec.begin())->renderInfo.mesh->GetAmbientExposure();;
				NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::EXPOSURE))->PushGraphicsData(&exposurrBuffer,
					sizeof(ExposureBuffer),
					static_cast<int>(CB_TYPE::EXPOSURE), false);

				auto& buffer = _buffers[instanceID];
				if (buffer->GetCount() > 0)
				{
					(*renderInfoVec.begin())->renderInfo.material->PushGraphicsData();
					buffer->PushData();
					(*renderInfoVec.begin())->renderInfo.mesh->Render((*renderInfoVec.begin())->renderInfo.materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
				}
			}
		}
	}
}

void InstancingManager::RenderSkinnedForward()
{
	ClearData();

	for (auto& pair : this->skinnedMeshForwardCache)
	{
		const std::set<std::shared_ptr<SkinnedRenderInfo>>& renderInfoVec = pair.second;

		const InstanceID instanceID = pair.first;

		{
			int descIndex = 0;
			int index = 0;
			for (auto& i : renderInfoVec)
			{
				if (i->renderInfo.isActive == false) continue;

				auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();
				auto aabb = i->renderInfo.mesh->GetBoundingBox(i->renderInfo.wtm, i->renderInfo.materialIndex);

				if (frustum.Intersects(aabb) == false)
				{
					continue;
				}

				const RenderInfo& renderInfo = i->renderInfo;
				InstancingData data;
				data.wtm = renderInfo.wtm;
				AddData(instanceID, data);
				this->instanceTransitionDesc->transitionDesc[descIndex++] = i->animator->GetTransitionDesc();

				lightMapUVBuffer->lightMapUV[index].lightMapIndex = renderInfo.lightMapIndex;
				lightMapUVBuffer->lightMapUV[index].scaling = renderInfo.uvScaling;
				lightMapUVBuffer->lightMapUV[index].uvOffset = renderInfo.uvOffset;

				index++;
			}

			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::LIGHTMAP_UV))->PushGraphicsData(lightMapUVBuffer.get(),
				sizeof(LightMapUVBuffer),
				static_cast<int>(CB_TYPE::LIGHTMAP_UV), false);

			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::INST_TRANSITION))->PushGraphicsData(this->instanceTransitionDesc.get(),
				sizeof(InstanceTransitionDesc), static_cast<int>(CB_TYPE::INST_TRANSITION));

			auto animationGroup = ResourceManager::Instance.Get().GetAnimationGroup((*renderInfoVec.begin())->modelName);
			animationGroup->Bind();

			if (renderInfoVec.size() != 0)
			{
				if ((*renderInfoVec.begin())->renderInfo.mesh == nullptr) continue;

				ExposureBuffer exposurrBuffer;
				exposurrBuffer.diffuseExposure = (*renderInfoVec.begin())->renderInfo.mesh->GetDiffuseExposure();
				exposurrBuffer.ambientExposure = (*renderInfoVec.begin())->renderInfo.mesh->GetAmbientExposure();;
				NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::EXPOSURE))->PushGraphicsData(&exposurrBuffer,
					sizeof(ExposureBuffer),
					static_cast<int>(CB_TYPE::EXPOSURE), false);

				auto& buffer = _buffers[instanceID];
				if (buffer->GetCount() > 0)
				{
					(*renderInfoVec.begin())->renderInfo.material->PushGraphicsData();
					buffer->PushData();
					(*renderInfoVec.begin())->renderInfo.mesh->Render((*renderInfoVec.begin())->renderInfo.materialIndex, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, buffer->GetCount(), buffer);
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

void InstancingManager::PopParticleSystem(ParticleSystem* system)
{
	particleRenderInfoMap.erase(system);
}

void InstancingManager::RenderParticle()
{
	MatrixBuffer matrixBuffer;
	matrixBuffer.VTM = CameraManager::Instance.Get().GetMainCamera()->GetVTM();
	matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM();

	for (auto& each : this->particleRenderInfoMap)
	{
		auto& tempList = each.second;

		int index = 0;
		for (auto& each2 : *tempList)
		{
			this->particleBuffer->particleDesc[index].pos = reinterpret_cast<DirectX::SimpleMath::Vector3&>(each2.position);
			this->particleBuffer->particleDesc[index].scale = each2.curScale;
			index++;
		}

		if (index != 0)
		{
			each.first->GetTexture()->Bind(0);

			static_cast<VertexShader*>(ResourceManager::Instance.Get().GetShader(L"ParticleVS.cso").get())->Bind();
			static_cast<GeometryShader*>(ResourceManager::Instance.Get().GetShader(L"ParticleGS.cso").get())->Bind();
			static_cast<PixelShader*>(ResourceManager::Instance.Get().GetShader(L"ParticlePS.cso").get())->Bind();

			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::PARTICLE))->PushGraphicsData(this->particleBuffer.get(),
				sizeof(ParticleBuffer),
				static_cast<int>(CB_TYPE::PARTICLE), true);

			matrixBuffer.WTM = DirectX::SimpleMath::Matrix::Identity;
			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX), true);

			ResourceManager::Instance.Get().GetMesh(L"Point")->Render(0, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, true, index);
		}
	}

	static_cast<VertexShader*>(ResourceManager::Instance.Get().GetShader(L"ParticleVS.cso").get())->UnBind();
	static_cast<GeometryShader*>(ResourceManager::Instance.Get().GetShader(L"ParticleGS.cso").get())->UnBind();
	static_cast<PixelShader*>(ResourceManager::Instance.Get().GetShader(L"ParticlePS.cso").get())->UnBind();
}

void InstancingManager::AddData(const InstanceID& id, InstancingData& instancingData)
{
	_buffers[id]->AddData(instancingData);
}

