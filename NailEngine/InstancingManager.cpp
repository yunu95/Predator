#include "InstancingManager.h"

#include "Material.h"
#include "Mesh.h"

#include "NailCamera.h"
#include "NailEngine.h"
#include "ConstantBuffer.h"
#include "ResourceManager.h"


LazyObjects<InstancingManager> InstancingManager::Instance;

void InstancingManager::RegisterMeshAndMaterial(std::vector<RenderInfo>& renderInfo)
{
	if (renderInfo.size() == 0) return;

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
			for (int i = 0; i < renderInfoVec.size(); ++i)
			{
				const RenderInfo& renderInfo = renderInfoVec[i];
				InstancingData data;
				data.wtm = renderInfo.wtm;
				AddData(instanceID, data);
			}

			auto& buffer = _buffers[instanceID];
			renderInfoVec[0].material->PushGraphicsData();
			auto test = renderInfoVec[0].mesh->GetMaterialCount();
			buffer->PushData();
			renderInfoVec[0].mesh->Render(renderInfoVec[0].materialIndex, buffer);
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

