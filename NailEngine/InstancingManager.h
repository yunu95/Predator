#pragma once
#include "Utils.h"

#include "InstanceBuffer.h"
#include "RenderSystem.h"

#include <map>
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
	void RegisterMeshAndMaterial(std::vector<RenderInfo>& renderInfo);
	void ClearData();

private:
	void AddData(InstanceID id, InstancingData instancingData);

private:

private:
	std::map<InstanceID/*instanceId*/, std::shared_ptr<InstanceBuffer>> _buffers;
};