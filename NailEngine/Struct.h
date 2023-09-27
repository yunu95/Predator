#pragma once

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

struct Vertex
{
	DirectX::SimpleMath::Vector4 pos;
	DirectX::SimpleMath::Vector4 color;
};

struct MatrixBuffer
{
	DirectX::SimpleMath::Matrix WTM;
	DirectX::SimpleMath::Matrix VTM;
	DirectX::SimpleMath::Matrix PTM;
};

struct MaterialBuffer
{
	DirectX::SimpleMath::Vector4 color{1.f,1.f,1.f,1.f};
};