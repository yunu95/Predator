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

class InstancingManager
{
public:
	static LazyObjects<InstancingManager> Instance;
	friend LazyObjects<InstancingManager>;

public:
	void Init();
	void ClearLightMapBuffer();
	bool IsInTree(std::shared_ptr<RenderInfo>& renderInfo);

	void SortByCameraDirection();

	void RenderStaticDeferred();
	void RenderStaticForward();
	void RenderDecal();

	void RenderStaticShadow();
	void RenderSkinnedShadow();
	void RenderStaticPointLightShadow(DirectX::SimpleMath::Matrix& lightWTM, PointLight* light);
	void RenderSkinnedPointLightShadow(DirectX::SimpleMath::Matrix& lightWTM, PointLight* light);

	void RegisterStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo);
	void RegisterStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo);
	void RegisterSkinnedDeferredData(std::shared_ptr<SkinnedRenderInfo>& renderInfo);
	void RegisterSkinnedForwardData(std::shared_ptr<SkinnedRenderInfo>& renderInfo);
	void RegisterDecalData(std::shared_ptr<RenderInfo>& renderInfo);

	void PopStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo);
	void PopStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo);
	void PopSkinnedDeferredData(std::shared_ptr<SkinnedRenderInfo>& renderInfo);
	void PopSkinnedForwardData(std::shared_ptr<SkinnedRenderInfo>& renderInfo);
	void PopDecalData(std::shared_ptr<RenderInfo>& renderInfo);

	void RegisterParticleRenderInfo(ParticleSystem* particleSystem, std::list<yunuGI::ParticleRenderInfo>* particleInfoList);

	void RenderSkinnedDeferred();
	void RenderSkinnedSilhouette();
	void RenderSkinnedForward();
	void ClearData();

	void PopParticleSystem(ParticleSystem* system);
	void RenderParticle();

	std::vector<std::pair<InstanceID, std::vector<std::shared_ptr<RenderInfo>>>>& GetStaticRenderInfoVec() { return staticMeshDeferredRenderVec; }

	QuadTree<RenderInfo>& GetQuadTree() { return this->quadTree; }

private:
	void AddData(const InstanceID& id, InstancingData& instancingData);

private:
	//
	std::map<InstanceID, unsigned int> staticMeshInstanceIDIndexMap;
	std::unordered_map<std::shared_ptr<RenderInfo>, unsigned int> staticMeshRenderInfoIndexMap;
	std::vector<std::pair<InstanceID, std::vector<std::shared_ptr<RenderInfo>>>> staticMeshDeferredRenderVec;
	//

	std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>>> staticMeshDeferredMap;
	std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>>> staticMeshForwardCache;

	std::map<InstanceID, std::set<std::shared_ptr<SkinnedRenderInfo>>> skinnedMeshDeferredCache;
	std::map<InstanceID, std::set<std::shared_ptr<SkinnedRenderInfo>>> skinnedMeshForwardCache;

	std::map<ParticleSystem*, std::list<yunuGI::ParticleRenderInfo>*> particleRenderInfoMap;
	std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>>> decalRenderInfoCache;

private:
	std::map<InstanceID, std::shared_ptr<InstanceBuffer>> _buffers;
	std::shared_ptr<InstanceTransitionDesc> instanceTransitionDesc;
	std::shared_ptr<ParticleBuffer> particleBuffer;
	std::shared_ptr<LightMapUVBuffer> lightMapUVBuffer;

	QuadTree<RenderInfo> quadTree{ DirectX::SimpleMath::Vector2{ 10000,10000 }, DirectX::SimpleMath::Vector2{ 10,10} };
};