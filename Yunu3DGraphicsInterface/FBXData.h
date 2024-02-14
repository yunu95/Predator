#pragma once

#include <vector>
#include <string>

namespace yunuGI
{
	struct BoneInfo
	{
		int index = -1;
	};

	struct MaterialData
	{
		std::wstring materialName;
		std::wstring albedoMap;
		std::wstring normalMap;
		std::wstring armMap;
		std::wstring emissionMap;
		std::wstring opacityMap;
		std::wstring heightMap;

		std::wstring temp0Map;
		std::wstring temp1Map;
		std::wstring temp2Map;
		std::wstring temp3Map;

		std::wstring vs{ L"DefaultVS.cso" };
		std::wstring ps{ L"DefaultPS.cso" };
	};

	struct BoundingBox
	{
		yunuGI::Vector3 maxPoint;
		yunuGI::Vector3 minPoint;
	};

	struct FBXData
	{
		std::vector<FBXData*> child;

		std::wstring nodeName;
		std::wstring meshName;

		yunuGI::Vector3 pos;
		yunuGI::Vector3 scale;
		yunuGI::Vector4 quat;

		std::vector<MaterialData> materialVec;

		BoneInfo boneInfo;

		bool hasAnimation;

		BoundingBox aabb;
	};


}
