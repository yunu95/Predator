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

// MeshID / MaterialID
using InstanceID = std::pair<unsigned __int64, unsigned __int64>;


class InstancingManager
{
public:
	static LazyObjects<InstancingManager> Instance;
	friend LazyObjects<InstancingManager>;

public:
	void Init();
	void RegisterStaticMeshAndMaterialInDeferred();
	void RegisterStaticMeshAndMaterialInForward();

	void RegisterMeshAndShadowMaterial(std::vector<RenderInfo>& renderInfo);

	void RegisterStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo);
	void RegisterStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo);
	void RegisterSkinnedData(std::shared_ptr<SkinnedRenderInfo>& renderInfo);

	void PopStaticDeferredData(std::shared_ptr<RenderInfo>& renderInfo);
	void PopStaticForwardData(std::shared_ptr<RenderInfo>& renderInfo);
	void PopSkinnedData(std::shared_ptr<SkinnedRenderInfo>& renderInfo);

	void RegisterSkinnedMeshAndMaterial();
	void ClearData();

private:
	void AddData(const InstanceID& id, InstancingData& instancingData);

private:
	std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>>> staticMeshDeferredCache;
	std::map<InstanceID, std::set<std::shared_ptr<RenderInfo>>> staticMeshForwardCache;

	std::map<InstanceID, std::set<std::shared_ptr<SkinnedRenderInfo>>> skinnedMeshCache;


private:
	std::map<InstanceID/*instanceId*/, std::shared_ptr<InstanceBuffer>> _buffers;
	std::shared_ptr<InstanceTransitionDesc> instanceTransitionDesc;


};