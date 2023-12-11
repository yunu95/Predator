#pragma once

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include <array>
#include <memory>

#define MAX_TEXTURE 8
#define MAX_INT 8

struct Vertex
{
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector4 color;
	DirectX::SimpleMath::Vector2 uv;
	DirectX::SimpleMath::Vector3 normal;
	DirectX::SimpleMath::Vector3 tangent;
	DirectX::SimpleMath::Vector4 index{0.f,0.f,0.f,0.f};
	DirectX::SimpleMath::Vector4 weights{0.f,0.f,0.f,0.f};

	bool isInit[4]{false,};

public:
	void FillBoneIndexWeight(unsigned int index, float weight)
	{
		if (!isInit[0])
		{
			isInit[0] = true;
			this->index.x = index;
			this->weights.x = weight;
		}
		else if (!isInit[1])
		{
			isInit[1] = true;
			this->index.y = index;
			this->weights.y = weight;
		}
		else if (!isInit[2])
		{
			isInit[2] = true;
			this->index.z = index;
			this->weights.z = weight;
		}
		else if (!isInit[3])
		{
			isInit[3] = true;
			this->index.w = index;
			this->weights.w = weight;
		}
	}
};

struct MatrixBuffer
{
	DirectX::SimpleMath::Matrix WTM;
	DirectX::SimpleMath::Matrix VTM;
	DirectX::SimpleMath::Matrix PTM;
	DirectX::SimpleMath::Matrix WVP;
	DirectX::SimpleMath::Matrix WorldInvTrans;
};

struct MaterialBuffer
{
	DirectX::SimpleMath::Vector4 color{1.f,1.f,1.f,1.f};

	std::array<unsigned int, MAX_TEXTURE> useTexture;
	std::array<int, MAX_INT> temp_int;
};

struct CameraBuffer
{
	DirectX::SimpleMath::Vector3 position;
	float padding;
};

struct BoneMatrix
{
	DirectX::SimpleMath::Matrix finalTM[256];
};

// Deferred Only
class Texture;
struct RenderTarget
{
	std::shared_ptr<Texture> texture;
	float clearColor[4] = { 0.f,0.f,0.f,0.f };
};

