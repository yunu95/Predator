#include "InstancingManager.h"

#include "Material.h"
#include "Mesh.h"


LazyObjects<InstancingManager> InstancingManager::Instance;

void InstancingManager::RegisterMeshAndMaterial(std::vector<RenderInfo>& renderInfo)
{
	ClearData();

	std::map<InstanceID, std::vector<RenderInfo>> cache;

	for (auto& each : renderInfo)
	{
		InstanceID instanceID = std::make_pair((unsigned __int64)each.mesh, (unsigned __int64)each.material);
		
		cache[instanceID].push_back(each);
	}

	for (auto& pair : cache)
	{
		const std::vector<RenderInfo>& renderInfoVec = pair.second;

		const InstanceID instanceID = pair.first;

		for (int i = 0; i < renderInfoVec.size(); ++i)
		{
			const RenderInfo& renderInfo = renderInfoVec[i];
			InstancingData data;
			data.wtm = renderInfo.wtm;
			AddData(instanceID, data);
		}

		auto& buffer = _buffers[instanceID];
		renderInfoVec[0].material->PushGraphicsData();
		for (int i = 0; i < renderInfoVec[0].mesh->GetMaterialCount(); ++i)
		{
			buffer->PushData();
			renderInfoVec[0].mesh->Render(buffer,i);
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

void InstancingManager::AddData(InstanceID id, InstancingData instancingData)
{
	if (_buffers.find(id) == _buffers.end())
	{
		_buffers[id] = std::make_shared<InstanceBuffer>();
	}

	_buffers[id]->AddData(instancingData);
}

