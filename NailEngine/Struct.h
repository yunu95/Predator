#pragma once

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include <array>

struct Vertex
{
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector4 color;
	DirectX::SimpleMath::Vector2 uv;
	DirectX::SimpleMath::Vector3 normal;
	DirectX::SimpleMath::Vector3 tangent;
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

	std::array<unsigned int, 7> useTexture;
};

struct CameraBuffer
{
	DirectX::SimpleMath::Vector3 position;
	float padding;
};