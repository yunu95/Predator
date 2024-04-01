#pragma once

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include <array>
#include <memory>

#define SM_SIZE 4096
#define PL_SM_SIZE 512

#define MAX_TEXTURE 18
#define MAX_INT 10
#define MAX_BONE_COUNT 250
#define MAX_FRAME_COUNT 500
#define MAX_INSTANCE_MODEL 500
#define MAX_POINT_LIGHT 10

struct Vertex
{
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector4 color{1,1,1,1};
	DirectX::SimpleMath::Vector2 uv;
	DirectX::SimpleMath::Vector3 normal;
	DirectX::SimpleMath::Vector3 tangent;
	unsigned int indices[4]{0xffffffffu,0xffffffffu ,0xffffffffu ,0xffffffffu };
	DirectX::SimpleMath::Vector4 weights{0.f,0.f,0.f,0.f};
	
public:
	void FillBoneIndexWeight(unsigned int index, float weight)
	{
		if (indices[0] == 0xffffffffu)
		{
			this->indices[0] = index;
			this->weights.x = weight;
		}
		else if (indices[1] == 0xffffffffu)
		{
			this->indices[1] = index;
			this->weights.y = weight;
		}
		else if (indices[2] == 0xffffffffu)
		{
			this->indices[2] = index;
			this->weights.z = weight;
		}
		else if (indices[3] == 0xffffffffu)
		{
			this->indices[3] = index;
			this->weights.w = weight;
		}
		else
		{
			assert(FALSE);
		}
	}
};

enum class CB_TYPE
{
	MATRIX,
	MATERIAL,
	LIGHT,
	CAMERA,
	BONE,
	INST_TRANSITION,
	FOG,
	POINTLIGHT_VPMATRIX,
	POINTLIGHT_INDEX,
	EXPOSURE,
	UTIL,
};

struct MatrixBuffer
{
	DirectX::SimpleMath::Matrix WTM;
	DirectX::SimpleMath::Matrix VTM;
	DirectX::SimpleMath::Matrix PTM;
	DirectX::SimpleMath::Matrix WVP;
	DirectX::SimpleMath::Matrix WorldInvTrans;
	DirectX::SimpleMath::Matrix VTMInv;
	DirectX::SimpleMath::Matrix lightVP;
	//DirectX::SimpleMath::Vector4 objectID;
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
	DirectX::SimpleMath::Matrix finalTM[MAX_BONE_COUNT];
};

struct KeyframeDesc
{
	int animIndex;
	unsigned int currFrame;
	unsigned int nextFrame;
	float ratio;
	float sumTime;
	float speed = 1.f;
	DirectX::SimpleMath::Vector2 padding;
};

struct TransitionDesc
{
	TransitionDesc()
	{
		curr.animIndex = -1;
		next.animIndex = -1;
	}

	void ClearNextAnimation()
	{
		next.animIndex = -1;
		next.currFrame = 0;
		next.nextFrame = 0;
		next.sumTime = 0;
		transitionTotalTime = 0;
		transitionRatio = 0;
	}

	float transitionDuration;
	float transitionSpeed;
	float transitionTotalTime;
	float transitionRatio;
	KeyframeDesc curr;
	KeyframeDesc next;
};

struct InstanceTransitionDesc
{
	TransitionDesc transitionDesc[MAX_INSTANCE_MODEL];
};

struct FogBuffer
{
	float start;
	float end;
	DirectX::SimpleMath::Vector2 padding;
};

struct PointLightVPMatrix
{
	DirectX::SimpleMath::Matrix viewProj[6];
};

struct PointLightIndex
{
	int plIndex;
	int isShadowCast;
	DirectX::SimpleMath::Vector2 padding;
};

class Mesh;
class Material;
struct RenderInfo
{
	Mesh* mesh;
	Material* material;
	unsigned int materialIndex;
	DirectX::SimpleMath::Matrix wtm;
	bool isActive = true;
};

class NailAnimator;
struct SkinnedRenderInfo
{
	RenderInfo renderInfo;
	std::wstring modelName;
	std::shared_ptr<NailAnimator> animator;
	TransitionDesc transitionDesc;
};

struct ExposureBuffer
{
	float diffuseExposure;
	float ambientExposure;
	DirectX::SimpleMath::Vector2 padding;
};

struct UtilBuffer
{
	float windowWidth;
	float windowHeight;
	float deltaTime;
	int useIBL;
};

// Deferred Only
class Texture;
struct RenderTarget
{
	std::shared_ptr<Texture> texture;
	float clearColor[4] = { 0.f,0.f,0.f,1.f };
};

