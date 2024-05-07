#pragma once
#include "Utils.h"

#include "InstanceBuffer.h"
#include "RenderSystem.h"
#include "QuadTree.h"

#include <map>
#include <set>
#include <memory>

class InstanceBuffer;
class Mesh;
class Material;
class PointLight;

// MeshID / MaterialID
//using InstanceID = std::pair<unsigned __int64, unsigned __int64>;
using InstanceID = std::pair<Mesh*, Material*>;

//struct BoundingBoxCompare
//{
//	bool operator()(const InstanceID& lhs, const InstanceID& rhs) const
//	{
//		// 바운딩 박스의 크기를 비교하여 정렬
//		auto lMesh = lhs.first;
//		auto rMesh = rhs.first;
//
//		auto& lBox = lMesh->GetOriginBoundingBox();
//		auto& rBox = rMesh->GetOriginBoundingBox();
//
//		float v1 = lBox.Extents.x * lBox.Extents.y * lBox.Extents.z;
//		float v2 = rBox.Extents.x * rBox.Extents.y * rBox.Extents.z;
//
//		return v1 > v2;
//	}
//};
//
//struct CameraDistanceCompare
//{
//	bool operator()(const std::shared_ptr<RenderInfo>& lhs, const std::shared_ptr<RenderInfo>& rhs) const
//	{
//		auto camera = CameraManager::Instance.Get().GetMainCamera();
//		auto vtm = camera->GetVTM();
//
//		DirectX::SimpleMath::Matrix lMat = lhs->wtm * vtm;
//		DirectX::SimpleMath::Matrix rMat = rhs->wtm * vtm;
//
//		DirectX::SimpleMath::Vector3 lPos = DirectX::SimpleMath::Vector3{ lMat._41, lMat._42 , lMat._43 };
//		DirectX::SimpleMath::Vector3 rPos = DirectX::SimpleMath::Vector3{ rMat._41, rMat._42 , rMat._43 };
//
//
//		float cameraX = 0.0f; 
//		float cameraY = 0.0f; 
//		float cameraZ = 0.0f; 
//
//		float dist1 = std::sqrt(std::pow(lPos.x - cameraX, 2) + std::pow(lPos.y - cameraY, 2) + std::pow(lPos.z - cameraZ, 2));
//		float dist2 = std::sqrt(std::pow(rPos.x - cameraX, 2) + std::pow(rPos.y - cameraY, 2) + std::pow(rPos.z - cameraZ, 2));
//
//		return dist1 < dist2;
//	}
//};

class InstancingManager
{
public:
	static LazyObjects<InstancingManager> Instance;
	friend LazyObjects<InstancingManager>;

public:
	void Init();

	void SortByCameraDirection();

	void RenderStaticDeferred();
	void RenderStaticForward();

	void RenderStaticShadow();
	void RenderStaticPointLightShadow(DirectX::SimpleMath::Matrix& lightWTM, PointLight* light);
	void RenderSkinnedPointLightShadow(DirectX::SimpleMath::Matrix& lightWTM, PointLight* light);

	void RegisterStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo);
	void RegisterStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo);
	void RegisterSkinnedData(std::shared_ptr<SkinnedRenderInfo>& renderInfo);

	void PopStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo);
	void PopStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo);
	void PopSkinnedData(std::shared_ptr<SkinnedRenderInfo>& renderInfo);

	void RegisterParticleRenderInfo(ParticleSystem* particleSystem, std::list<yunuGI::ParticleRenderInfo>* particleInfoList);

	void RenderSkinned();
	void ClearData();

	void PopParticleSystem(ParticleSystem* system);
	void RenderParticle();

	std::vector<std::pair<InstanceID, std::vector<std::shared_ptr<RenderInfo>>>>& GetStaticRenderInfoVec() { return staticMeshDeferredRenderVec; }

private:
	void AddData(const InstanceID& id, InstancingData& instancingData);

private:
	//std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>, CameraDistanceCompare>, BoundingBoxCompare> staticMeshDeferredCache;

	std::map<InstanceID, unsigned int> staticMeshInstanceIDIndexMap;
	std::unordered_map<std::shared_ptr<RenderInfo>, unsigned int> staticMeshRenderInfoIndexMap;

	std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>>> staticMeshDeferredMap;
	std::vector<std::pair<InstanceID, std::vector<std::shared_ptr<RenderInfo>>>> staticMeshDeferredRenderVec;
	// 

	std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>>> staticMeshForwardCache;

	std::map<InstanceID, std::set<std::shared_ptr<SkinnedRenderInfo>>> skinnedMeshCache;

	std::map<ParticleSystem*, std::list<yunuGI::ParticleRenderInfo>*> particleRenderInfoMap;

private:
	std::map<InstanceID, std::shared_ptr<InstanceBuffer>> _buffers;
	std::shared_ptr<InstanceTransitionDesc> instanceTransitionDesc;
	std::shared_ptr<ParticleBuffer> particleBuffer;
	std::shared_ptr<LightMapUVBuffer> lightMapUVBuffer;

	QuadTree<RenderInfo> quadTree{ DirectX::SimpleMath::Vector2{ 10000,10000 }, DirectX::SimpleMath::Vector2{ 10,10} };
};