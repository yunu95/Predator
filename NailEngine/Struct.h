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

// Deferred Only
class Texture;
struct RenderTarget
{
	std::shared_ptr<Texture> texture;
	float clearColor[4] = { 0.f,0.f,0.f,0.f };
};

