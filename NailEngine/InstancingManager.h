#pragma once
#include "Utils.h"

#include "InstanceBuffer.h"
#include "RenderSystem.h"

#include <map>
#include <set>
#include <memory>

class InstanceBuffer;
class Mesh;
class Material;
class PointLight;

// MeshID / MaterialID
using InstanceID = std::pair<unsigned __int64, unsigned __int64>;


class InstancingManager
{
public:
	static LazyObjects<InstancingManager> Instance;
	friend LazyObjects<InstancingManager>;

public:
	void Init();
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

	void RenderParticle();

private:
	void AddData(const InstanceID& id, InstancingData& instancingData);

private:
	std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>>> staticMeshDeferredCache;
	std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>>> staticMeshForwardCache;

	std::map<InstanceID, std::set<std::shared_ptr<SkinnedRenderInfo>>> skinnedMeshCache;

	std::map<ParticleSystem*, std::list<yunuGI::ParticleRenderInfo>*> particleRenderInfoMap;

private:
	std::map<InstanceID, std::shared_ptr<InstanceBuffer>> _buffers;
	std::shared_ptr<InstanceTransitionDesc> instanceTransitionDesc;
	std::shared_ptr<ParticleBuffer> particleBuffer;
	std::shared_ptr<LightMapUVBuffer> lightMapUVBuffer;
};