#include "GraphicsPch.h"
#include "RenderSystem.h"

#include "Mesh.h"
#include "Material.h"
#include "ResourceManager.h"
#include "RenderTargetGroup.h"
#include "SwapChain.h"
#include "CameraManager.h"
#include "FFCamera.h"
#include "ConstantBuffer.h"
#include "InstancingBuffer.h"
#include "Shader.h"

#include "StructuredBuffer.h"

#include "Frustum.h"
#include "TimeManager.h"

#include "CommandQueue.h"

DirectX::BoundingSphere bound;

XMVECTOR lightDir;
XMVECTOR lightPos;
XMVECTOR targetPos;
XMVECTOR lightUp;
XMMATRIX lightView;

RenderSystem::~RenderSystem()
{
	for (auto& buffer : m_buffers)
	{
		delete buffer.second;
	}
}

void RenderSystem::Init(Device* p_device, SwapChain* p_swapChain, array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* p_renderTargets, vector<ConstantBuffer*>* p_constantBuffers)
{
	bound.Center = Vec3(0, 0, 0);
	bound.Radius = sqrt(45 * 45);

	m_device = p_device;
	m_swapChain = p_swapChain;
	m_renderTargets = p_renderTargets;
	m_constantBuffers = p_constantBuffers;
}

void RenderSystem::PushData(const Matrix& p_WTM, const wstring& p_mesh, const MATERIAL_DESC& p_materialDesc, wstring& p_materialName, int p_materialIndex)
{
	tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int> _data = { p_WTM, p_mesh, p_materialDesc,p_materialName,p_materialIndex };
	m_renderObjects.emplace_back(_data);
}

void RenderSystem::PushParticleData(const Matrix& p_WTM, float p_deltaTime, unsigned __int32 p_maxParticle, float p_createInterval, float p_accTime, float p_minLifeTime, float p_maxLifeTime, float p_minSpeed, float p_maxSpeed, float p_startScale, float p_endScale)
{
	tuple<Matrix, float, unsigned __int32, float, float, float, float, float, float, float, float> _data =
	{
		p_WTM,
		p_deltaTime,
		p_maxParticle,
		p_createInterval,
		p_accTime,
		p_minLifeTime,
		p_maxLifeTime,
		p_minSpeed,
		p_maxSpeed,
		p_startScale,
		p_endScale
	};

	particle.emplace_back(_data);
}

void RenderSystem::Render()
{
	// ·»´õ Å¸°Ùµé Å¬¸®¾î
	ClearRTV();

	// ¿ÀºêÁ§Æ® Á¤·Ä
	SortObject();

	// ±×¸²ÀÚ ·»´õ (½¦µµ¿ì ¸Ê ¸¸µé±âÀÓ)
	RenderShadow();

	// µðÆÛµå ·»´õ
	Render_Deferred();

	// ½ºÅ²µå ¸Þ½¬ ·»´õ
	Render_SkinnedMesh();

	// ¶óÀÌÆ® ·»´õ
	Render_Light();

	// ÃÖÁ¾ È­¸é Ãâ·Â
	Render_Final();

	// Æ÷¿öµå ·»´õ
	Render_Forward();

	// ÆÄÆ¼Å¬ ·»´õ
	Render_Particle();

	// ÀÌÆåÆ® ·»´õ
	Render_Effect();

	// UI ·»´õ
	Render_UI();

	// Æ÷½ºÆ® ÇÁ·Î¼¼½Ì
	Render_Post();

	m_skinnedMeshTest.clear();
	m_effect.clear();
	m_deferred_skin_shadow.clear();
	particle.clear();
}

void RenderSystem::ClearInstanceBuffer()
{
	for (auto& buffer : m_buffers)
	{
		buffer.second->Clear();
	}
}

void RenderSystem::ClearRenderObjects()
{
	m_renderObjects.clear();
}

void RenderSystem::DrawSkinnedMesh(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio, const std::wstring& p_currentAnimationName, const Matrix& p_WTM, const std::wstring& p_mesh, const MATERIAL_DESC& p_material, std::wstring& p_materialName, int p_materialIndex)
{
	SkinnedMeshData temp =
	{
		p_id,
		p_frame,
		p_nextFrame,
		p_frameRatio,
		p_currentAnimationName,
		p_WTM,
		p_mesh,
		p_material,
		p_materialName,
		p_materialIndex
	};

	Shader* shader = ResourceManager::GetInstance()->GetShader(p_material.shader);
	switch (shader->GetShaderInfo().shaderType)
	{

	case SHADER_TYPE::EFFECT:
	{
		m_effect.emplace_back(temp);
	}
	break;
	case SHADER_TYPE::DEFFERRED_SKIN_SHADOW:
	{
		m_deferred_skin_shadow.emplace_back(temp);
	}
	break;
	case SHADER_TYPE::DEFERRED_AND_SKIN:
	{
		m_skinnedMeshTest.emplace_back(temp);
	}
	break;
	}
}

void RenderSystem::ClearRTV()
{
	int8 backIndex = m_swapChain->GetBackBufferIndex();

	// ¶óÀÌÆ® Á¤º¸ Push

	// ·»´õ Å¸°Ùµé Clear
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)]->ClearRenderTargetView(backIndex);
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->ClearRenderTargetView();
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->ClearRenderTargetView();
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)]->ClearRenderTargetView();
}

void RenderSystem::SortObject()
{
	m_deferred.clear();
	m_deferred_shadow.clear();
	m_forward.clear();
	m_lights.clear();
	m_ui.clear();
	m_textureAnim.clear();

	for (auto& object : m_renderObjects)
	{
		switch (ResourceManager::GetInstance()->GetShader(get<2>(object).shader)->GetShaderInfo().shaderType)
		{
		case SHADER_TYPE::EFFECT_ANIM:
		{
			m_textureAnim.emplace_back(object);
		}
		break;

		case SHADER_TYPE::DEFERRED:
		{
			m_deferred.emplace_back(object);
		}
		break;

		case SHADER_TYPE::DEFFERRED_SHADOW:
		{
			m_deferred_shadow.emplace_back(object);
		}
		break;

		case SHADER_TYPE::FORWARD:
		{
			m_forward.emplace_back(object);
		}
		break;

		case SHADER_TYPE::LIGHTING:
		{
			m_lights.emplace_back(object);
		}
		break;

		case SHADER_TYPE::PARTICLE:
		{
		}
		break;

		case SHADER_TYPE::UI:
		{
			m_ui.emplace_back(object);
		}
		break;
		}
	}
}

void RenderSystem::RenderShadow()
{
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)]->OMSetRenderTargets();

	Frustum* frustum = CameraManager::GetInstance().GetCamera(0)->GetFrustum();

	for (int i = 0; i < m_lightParams->lightCount; ++i)
	{
		if (static_cast<LIGHT_TYPE>(m_lightParams->lights[i].lightType) != LIGHT_TYPE::DIRECTIONAL_LIGHT)
		{
			continue;
		}

		Matrix lightWorld = get<0>(m_lights[i]);

		Vec3 dir = lightWorld.Backward();

#pragma region TEST
		{
			lightDir = XMLoadFloat3(&dir);
			lightPos = -2.0f * bound.Radius * lightDir;
			targetPos = XMLoadFloat3(&bound.Center);
			lightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);
		}
#pragma  endregion

		XMFLOAT3 sphereCenterLS;
		::XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

		float l = sphereCenterLS.x - bound.Radius;
		float b = sphereCenterLS.y - bound.Radius;
		float n = sphereCenterLS.z - bound.Radius;
		float r = sphereCenterLS.x + bound.Radius;
		float t = sphereCenterLS.y + bound.Radius;
		float f = sphereCenterLS.z + bound.Radius;

		XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);


		for (auto& i : m_deferred_shadow)
		{
			if (!frustum->ContainSphere(get<0>(i), ResourceManager::GetInstance()->GetMesh(get<1>(i))->GetBoundRadius()))
			{
				continue;
			}

			TransformParams transformParams = {};
			transformParams.matWorld = get<0>(i);
			transformParams.matView = lightView;
			transformParams.matProjection = lightProj;
			transformParams.matWV = transformParams.matWorld * transformParams.matView;
			transformParams.matWVP = transformParams.matWV * transformParams.matProjection;

			(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

			ResourceManager::GetInstance()->GetMaterial(L"Shadow")->PushGraphicsData();

			ResourceManager::GetInstance()->GetMesh(get<1>(i))->Render(false, 1, get<4>(i));
		}

		for (auto& i : m_deferred_skin_shadow)
		{
			unsigned __int64 id = i.animatorID;
			__int32 frame = i.curFrame;
			__int32 nextFrame = i.nextFrame;
			float frameRatio = i.ratio;
			wstring curAnimName = i.curAnimationName;
			Matrix wtm = i.WTM;
			wstring meshName = i.meshName;
			MATERIAL_DESC desc = i.materialDesc;
			wstring& materialName = i.materialName;
			int materialIndex = i.materialIndex;

			if (desc.vec4Params[0].x == 1.f) continue;

			Mesh* mesh = ResourceManager::GetInstance()->GetMesh(meshName);
			Material* material = ResourceManager::GetInstance()->GetMaterial(desc, materialName);

			if (!frustum->ContainSphere(wtm, mesh->GetBoundRadius()))
			{
				continue;
			}

			map<uint64, pair<Material*, StructuredBuffer*>>& mapAnimData = ResourceManager::GetInstance()->GetAnimData();

			auto iter = mapAnimData.find(id);

			if (iter == mapAnimData.end())
			{
				Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeAnimation");
				Material* material = new Material(m_device, m_constantBuffers,
					ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
					ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
					ResourceManager::GetInstance()->GetComputeCommandQueue());

				material->SetShader(shader);
				mapAnimData.insert({ id, {material, new StructuredBuffer()} });
			}



			uint32 boneCount = mesh->GetBoneInfo().size();

			if (mapAnimData[id].second->GetElementCount() < boneCount)
			{
				mapAnimData[id].second->Init(sizeof(Matrix), boneCount, m_device,
					ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
					ResourceManager::GetInstance()->GetComputeDescriptorHeap());
			}

			FFMeshData& meshData = ResourceManager::GetInstance()->GetMeshDataFromMeshName(meshName);

			uint32 animIndex = 0;
			for (size_t i = 0; i < meshData.vecAnimClip.size(); ++i)
			{
				if (curAnimName == meshData.vecAnimClip[i].animName)
				{
					animIndex = i;
					break;
				}
			}

			mesh->GetBoneFrameDataBuffer(animIndex)->PushComputeSRVData(SRV_REGISTER::t8);
			mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

			mapAnimData[id].second->PushComputeUAVData(UAV_REGISTER::u0);

			mapAnimData[id].first->SetInt(0, boneCount);
			mapAnimData[id].first->SetInt(1, frame);
			mapAnimData[id].first->SetInt(2, nextFrame);
			mapAnimData[id].first->SetFloat(0, frameRatio);

			uint32 groupCount = (boneCount / 256) + 1;
			mapAnimData[id].first->Dispatch(groupCount, 1, 1);

			mapAnimData[id].second->PushGraphicsData(SRV_REGISTER::t7);

			TransformParams transformParams = {};
			transformParams.matWorld = wtm;
			transformParams.matView = lightView;
			transformParams.matProjection = lightProj;
			transformParams.matWV = transformParams.matWorld * transformParams.matView;
			transformParams.matWVP = transformParams.matWV * transformParams.matProjection;

			(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

			ResourceManager::GetInstance()->GetMaterial(L"ShadowSkinned")->PushGraphicsData();
			mesh->Render(false, 1, materialIndex);
		}
	}

	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)]->WaitTargetToResource();
}

void RenderSystem::Render_Deferred()
{
	// µðÆÛµå¿ë ·»´õ Å¸°Ù Set
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->OMSetRenderTargets();
	Render_Object(m_deferred);
	Render_Object(m_deferred_shadow);
}

void RenderSystem::Render_SkinnedMesh()
{
	Frustum* frustum = CameraManager::GetInstance().GetCamera(0)->GetFrustum();

	for (auto& i : m_skinnedMeshTest)
	{

		unsigned __int64 id = i.animatorID;
		__int32 frame = i.curFrame;
		__int32 nextFrame = i.nextFrame;
		float frameRatio = i.ratio;
		wstring curAnimName = i.curAnimationName;
		Matrix wtm = i.WTM;
		wstring meshName = i.meshName;
		MATERIAL_DESC desc = i.materialDesc;
		wstring& materialName = i.materialName;
		int materialIndex = i.materialIndex;

		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(meshName);
		Material* material = ResourceManager::GetInstance()->GetMaterial(desc, materialName);

		if (!frustum->ContainSphere(wtm, mesh->GetBoundRadius()))
		{
			continue;
		}

		map<uint64, pair<Material*, StructuredBuffer*>>& mapAnimData = ResourceManager::GetInstance()->GetAnimData();

		auto iter = mapAnimData.find(id);

		if (iter == mapAnimData.end())
		{
			Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeAnimation");
			Material* material = new Material(m_device, m_constantBuffers,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeCommandQueue());

			material->SetShader(shader);
			mapAnimData.insert({ id, {material, new StructuredBuffer()} });
		}

		uint32 boneCount = mesh->GetBoneInfo().size();

		if (mapAnimData[id].second->GetElementCount() < boneCount)
		{
			mapAnimData[id].second->Init(sizeof(Matrix), boneCount, m_device,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap());
		}

		FFMeshData& meshData = ResourceManager::GetInstance()->GetMeshDataFromMeshName(meshName);

		uint32 animIndex = 0;
		for (size_t i = 0; i < meshData.vecAnimClip.size(); ++i)
		{
			if (curAnimName == meshData.vecAnimClip[i].animName)
			{
				animIndex = i;
				break;
			}
		}

		mesh->GetBoneFrameDataBuffer(animIndex)->PushComputeSRVData(SRV_REGISTER::t8);
		mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

		mapAnimData[id].second->PushComputeUAVData(UAV_REGISTER::u0);

		mapAnimData[id].first->SetInt(0, boneCount);
		mapAnimData[id].first->SetInt(1, frame);
		mapAnimData[id].first->SetInt(2, nextFrame);
		mapAnimData[id].first->SetFloat(0, frameRatio);

		uint32 groupCount = (boneCount / 256) + 1;
		mapAnimData[id].first->Dispatch(groupCount, 1, 1);

		mapAnimData[id].second->PushGraphicsData(SRV_REGISTER::t7);

		TransformParams transformParams = {};
		transformParams.matWorld = wtm;
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		material->PushGraphicsData();
		mesh->Render(false, 1, materialIndex);
	}

	for (auto& i : m_deferred_skin_shadow)
	{
		unsigned __int64 id = i.animatorID;
		__int32 frame = i.curFrame;
		__int32 nextFrame = i.nextFrame;
		float frameRatio = i.ratio;
		wstring curAnimName = i.curAnimationName;
		Matrix wtm = i.WTM;
		wstring meshName = i.meshName;
		MATERIAL_DESC desc = i.materialDesc;
		wstring& materialName = i.materialName;
		int materialIndex = i.materialIndex;

		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(meshName);
		Material* material = ResourceManager::GetInstance()->GetMaterial(desc, materialName);

		if (!frustum->ContainSphere(wtm, mesh->GetBoundRadius()))
		{
			continue;
		}

		map<uint64, pair<Material*, StructuredBuffer*>>& mapAnimData = ResourceManager::GetInstance()->GetAnimData();

		auto iter = mapAnimData.find(id);

		if (iter == mapAnimData.end())
		{
			Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeAnimation");
			Material* material = new Material(m_device, m_constantBuffers,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeCommandQueue());

			material->SetShader(shader);
			mapAnimData.insert({ id, {material, new StructuredBuffer()} });
		}

		uint32 boneCount = mesh->GetBoneInfo().size();

		if (mapAnimData[id].second->GetElementCount() < boneCount)
		{
			mapAnimData[id].second->Init(sizeof(Matrix), boneCount, m_device,
				ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap());
		}

		FFMeshData& meshData = ResourceManager::GetInstance()->GetMeshDataFromMeshName(meshName);

		uint32 animIndex = 0;
		for (size_t i = 0; i < meshData.vecAnimClip.size(); ++i)
		{
			if (curAnimName == meshData.vecAnimClip[i].animName)
			{
				animIndex = i;
				break;
			}
		}

		mesh->GetBoneFrameDataBuffer(animIndex)->PushComputeSRVData(SRV_REGISTER::t8);
		mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

		mapAnimData[id].second->PushComputeUAVData(UAV_REGISTER::u0);

		mapAnimData[id].first->SetInt(0, boneCount);
		mapAnimData[id].first->SetInt(1, frame);
		mapAnimData[id].first->SetInt(2, nextFrame);
		mapAnimData[id].first->SetFloat(0, frameRatio);

		uint32 groupCount = (boneCount / 256) + 1;
		mapAnimData[id].first->Dispatch(groupCount, 1, 1);

		mapAnimData[id].second->PushGraphicsData(SRV_REGISTER::t7);

		TransformParams transformParams = {};
		transformParams.matWorld = wtm;
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		material->PushGraphicsData();
		mesh->Render(false, 1, materialIndex);
	}

	// µðÆÛµå¿ë ·»´õ Å¸°Ù Wait
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::G_BUFFER)]->WaitTargetToResource();
}

void RenderSystem::Render_Light()
{
	// ¶óÀÌÆ®¿ë ·»´õ Å¸°Ù Set
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->OMSetRenderTargets();

	for (int i = 0; i < m_lightParams->lightCount; ++i)
	{
		if (static_cast<LIGHT_TYPE>(m_lightParams->lights[i].lightType) != LIGHT_TYPE::DIRECTIONAL_LIGHT)
		{
			continue;
		}

		Matrix lightWorld = get<0>(m_lights[i]);

		Vec3 dir = lightWorld.Backward();

#pragma region TEST
		{
			lightDir = XMLoadFloat3(&dir);
			lightPos = -2.0f * bound.Radius * lightDir;
			targetPos = XMLoadFloat3(&bound.Center);
			lightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);
		}
#pragma  endregion

		XMFLOAT3 sphereCenterLS;
		::XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

		float l = sphereCenterLS.x - bound.Radius;
		float b = sphereCenterLS.y - bound.Radius;
		float n = sphereCenterLS.z - bound.Radius;
		float r = sphereCenterLS.x + bound.Radius;
		float t = sphereCenterLS.y + bound.Radius;
		float f = sphereCenterLS.z + bound.Radius;

		XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

		TransformParams transformParams = {};
		transformParams.matWorld = get<0>(m_lights[i]);
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = transformParams.matView.Invert();
		transformParams.matLightVP = lightView * lightProj;

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		get<2>(m_lights[i]).intParams[0] = 0;
		get<2>(m_lights[i]).texture[2] = L"ShadowTarget";
		get<2>(m_lights[i]).texture[3] = L"DepthTarget";
		

		ResourceManager::GetInstance()->GetMaterial(get<2>(m_lights[i]), get<3>(m_lights[i]))->PushGraphicsData();
		ResourceManager::GetInstance()->GetMesh(get<1>(m_lights[i]))->Render();
	}

	// ¶óÀÌÆ®¿ë ·»´õ Å¸°Ù Wait
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->WaitTargetToResource();
}

void RenderSystem::Render_Final()
{
	int8 backIndex = m_swapChain->GetBackBufferIndex();
	// ½º¿ÒÃ¼ÀÎÀÇ ¹é¹öÆÛ Set
	(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)]->OMSetRenderTargets(1, backIndex);

	ResourceManager::GetInstance()->GetMaterial(L"FinalMaterial")->PushGraphicsData();
	ResourceManager::GetInstance()->GetMesh(L"Rectangle")->Render(true);
}

void RenderSystem::Render_Forward()
{
	Render_Object(m_forward);
}

void RenderSystem::Render_UI()
{
	for (auto& i : m_ui)
	{
		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(get<1>(i));
		Material* material = ResourceManager::GetInstance()->GetMaterial(get<2>(i), get<3>(i));

		TransformParams transformParams = {};
		transformParams.matWorld = get<0>(i);
		transformParams.matView = Matrix::Identity;//CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matOrthographicProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		material->PushGraphicsData();
		mesh->Render();
	}
}

void RenderSystem::Render_Post()
{
	ResourceManager::GetInstance()->GetMaterial(L"PostMaterial")->PushGraphicsData();
	ResourceManager::GetInstance()->GetMesh(L"Rectangle")->Render();
}

void RenderSystem::Render_Object(vector<tuple<Matrix, wstring, MATERIAL_DESC, wstring&, int>>& p_renderObjects)
{
	Frustum* frustum = CameraManager::GetInstance().GetCamera(0)->GetFrustum();

	for (auto& object : p_renderObjects)
	{
		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(get<1>(object));
		Material* material = ResourceManager::GetInstance()->GetMaterial(get<2>(object), get<3>(object));

		if (!frustum->ContainSphere(get<0>(object), mesh->GetBoundRadius()))
		{
			continue;
		}

		TransformParams transformParams = {};
		transformParams.matWorld = get<0>(object);
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		material->PushGraphicsData();
		mesh->Render(false, 1, get<4>(object));
	}
}

void RenderSystem::Render_Particle()
{
	for (auto& object : particle)
	{
		Matrix wtm = get<0>(object);
		float deltaTime = get<1>(object);
		unsigned __int32 maxParticle = get<2>(object);
		float createInterval = get<3>(object);
		float accTime = get<4>(object);
		float minLifeTime = get<5>(object);
		float maxLifeTime = get<6>(object);
		float minSpeed = get<7>(object);
		float maxSpeed = get<8>(object);
		float startScale = get<9>(object);
		float endScale = get<10>(object);

		if (_particleBuffer == nullptr)
		{
			_particleBuffer = new StructuredBuffer();
			_particleBuffer->Init(sizeof(ParticleInfo), maxParticle,
				m_device, ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap());
		}
		if (_computeSharedBuffer == nullptr)
		{
			_computeSharedBuffer = new StructuredBuffer();
			_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1,
				m_device, ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
				ResourceManager::GetInstance()->GetComputeDescriptorHeap());
		}

		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(L"Point");
		Material* material = ResourceManager::GetInstance()->GetMaterial(L"Particle");

		Material* computeMaterial = ResourceManager::GetInstance()->GetMaterial(L"ParticleCompute");

		int32 add = 0;
		if (createInterval < accTime)
		{
			add = 1;
		}

		_particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
		_computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

		computeMaterial->SetInt(0, maxParticle);
		computeMaterial->SetInt(1, add);

		computeMaterial->SetVec2(1, Vec2(deltaTime, accTime));
		computeMaterial->SetVec4(0, Vec4(minLifeTime, maxLifeTime, minSpeed, maxSpeed));

		computeMaterial->Dispatch(1, 1, 1);

		TransformParams transformParams = {};
		transformParams.matWorld = get<0>(object);
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		_particleBuffer->PushGraphicsData(SRV_REGISTER::t8);
		material->SetFloat(0, startScale);
		material->SetFloat(1, endScale);
		material->PushGraphicsData();
		mesh->Render(false, maxParticle);
	}
}

void RenderSystem::Render_Effect()
{
	Frustum* frustum = CameraManager::GetInstance().GetCamera(0)->GetFrustum();

	// Effect Texture Animation
	for (auto& i : m_textureAnim)
	{
		Mesh* mesh = ResourceManager::GetInstance()->GetMesh(get<1>(i));
		Material* material = ResourceManager::GetInstance()->GetMaterial(get<2>(i), get<3>(i));

		if (!frustum->ContainSphere(get<0>(i), mesh->GetBoundRadius()))
		{
			continue;
		}

		TransformParams transformParams = {};
		transformParams.matWorld = get<0>(i);
		transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
		transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
		transformParams.matWV = transformParams.matWorld * transformParams.matView;
		transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
		transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

		(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

		//material->GetMaterialDesc().floatParams[1] = TimeManager::GetInstance()->GetAccTIme();
		//material->SetMaterialPramsForDesc(material->GetMaterialDesc());
		material->PushGraphicsData();
		mesh->Render(false, 1, get<4>(i));
	}

	//// Sub Effect Skinning Animation
	//for (auto& i : m_effect)
	//{
	//	Frustum* frustum = CameraManager::GetInstance().GetCamera(0)->GetFrustum();

	//	unsigned __int64 id = i.animatorID;
	//	__int32 frame = i.curFrame;
	//	__int32 nextFrame = i.nextFrame;
	//	float frameRatio = i.ratio;
	//	wstring curAnimName = i.curAnimationName;
	//	Matrix wtm = i.WTM;
	//	wstring meshName = i.meshName;
	//	MATERIAL_DESC desc = i.materialDesc;
	//	wstring& materialName = i.materialName;
	//	int materialIndex = i.materialIndex;

	//	Mesh* mesh = ResourceManager::GetInstance()->GetMesh(meshName);
	//	Material* material = ResourceManager::GetInstance()->GetMaterial(desc, materialName);

	//	if (!frustum->ContainSphere(wtm, mesh->GetBoundRadius()))
	//	{
	//		continue;
	//	}

	//	map<uint64, pair<Material*, StructuredBuffer*>>& mapAnimData = ResourceManager::GetInstance()->GetAnimData();

	//	auto iter = mapAnimData.find(id);

	//	if (iter == mapAnimData.end())
	//	{
	//		Shader* shader = ResourceManager::GetInstance()->GetShader(L"ComputeAnimation");
	//		Material* material = new Material(m_device, m_constantBuffers,
	//			ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
	//			ResourceManager::GetInstance()->GetComputeDescriptorHeap(),
	//			ResourceManager::GetInstance()->GetComputeCommandQueue());

	//		material->SetShader(shader);
	//		mapAnimData.insert({ id, {material, new StructuredBuffer()} });
	//	}

	//	uint32 boneCount = mesh->GetBoneInfo().size();

	//	if (mapAnimData[id].second->GetElementCount() < boneCount)
	//	{
	//		mapAnimData[id].second->Init(sizeof(Matrix), boneCount, m_device,
	//			ResourceManager::GetInstance()->GetGraphicsDescriptorHeap(),
	//			ResourceManager::GetInstance()->GetComputeDescriptorHeap());
	//	}

	//	FFMeshData& meshData = ResourceManager::GetInstance()->GetMeshDataFromMeshName(meshName);

	//	uint32 animIndex = 0;
	//	for (size_t i = 0; i < meshData.vecAnimClip.size(); ++i)
	//	{
	//		if (curAnimName == meshData.vecAnimClip[i].animName)
	//		{
	//			animIndex = i;
	//			break;
	//		}
	//	}

	//	mesh->GetBoneFrameDataBuffer(animIndex)->PushComputeSRVData(SRV_REGISTER::t8);
	//	mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

	//	mapAnimData[id].second->PushComputeUAVData(UAV_REGISTER::u0);

	//	mapAnimData[id].first->SetInt(0, boneCount);
	//	mapAnimData[id].first->SetInt(1, frame);
	//	mapAnimData[id].first->SetInt(2, nextFrame);
	//	mapAnimData[id].first->SetFloat(0, frameRatio);

	//	uint32 groupCount = (boneCount / 256) + 1;
	//	mapAnimData[id].first->Dispatch(groupCount, 1, 1);

	//	mapAnimData[id].second->PushGraphicsData(SRV_REGISTER::t7);

	//	TransformParams transformParams = {};
	//	transformParams.matWorld = wtm;
	//	transformParams.matView = CameraManager::GetInstance().GetCamera(0)->matView;
	//	transformParams.matProjection = CameraManager::GetInstance().GetCamera(0)->matPerspectiveProjection;
	//	transformParams.matWV = transformParams.matWorld * transformParams.matView;
	//	transformParams.matWVP = transformParams.matWV * transformParams.matProjection;
	//	transformParams.matViewInv = CameraManager::GetInstance().GetCamera(0)->matView.Invert();

	//	(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->PushGraphicsData(&transformParams, sizeof(transformParams));

	//	material->PushGraphicsData();
	//	mesh->Render(false, 1, materialIndex);
	//}
}

void RenderSystem::AddInstanceParams(uint64 p_instanceID, InstancingParams& p_params)
{
	if (m_buffers.find(p_instanceID) == m_buffers.end())
	{
		m_buffers[p_instanceID] = new InstancingBuffer(m_device);
	}

	m_buffers[p_instanceID]->AddParams(p_params);
}
