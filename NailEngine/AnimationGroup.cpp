#include "AnimationGroup.h"

#include "ResourceManager.h"
#include "ResourceBuilder.h"
#include "Device.h"

#include "Animation.h"

void AnimationGroup::CreateTexture()
{
	int animationCount = this->animationVec.size();

	this->animTransformVec.resize(animationCount);
	for (int i = 0; i < animationCount; i++)
	{
		CreateAnimationTransform(i);
	}
	
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_BONE_COUNT * 4;
		desc.Height = MAX_FRAME_COUNT;
		desc.ArraySize = animationCount;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16바이트
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		// 16000
		const unsigned int dataSize = MAX_BONE_COUNT * sizeof(DirectX::SimpleMath::Matrix);
		// 8000000
		const unsigned int pageSize = dataSize * MAX_FRAME_COUNT;
		// mallocPtr이 가리키는 공간이 8000000
		void* mallocPtr = ::malloc(pageSize * animationCount);

		for (int c = 0; c < animationCount; c++)
		{
			unsigned int startOffset = c * pageSize;

			BYTE* pageStartPtr = reinterpret_cast<BYTE*>(mallocPtr) + startOffset;

			for (unsigned int f = 0; f < MAX_FRAME_COUNT; f++)
			{
				void* ptr = pageStartPtr + (dataSize * f);
				::memcpy(ptr, this->animTransformVec[c].transforms[f].data(), dataSize);
			}
		}

		// 리소스 만들기
		std::vector<D3D11_SUBRESOURCE_DATA> subResources(animationCount);

		for (int c = 0; c < animationCount; c++)
		{
			void* ptr = (BYTE*)mallocPtr + c * pageSize;
			subResources[c].pSysMem = ptr;
			subResources[c].SysMemPitch = dataSize;
			subResources[c].SysMemSlicePitch = pageSize;
		}

		ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&desc, subResources.data(), texture.GetAddressOf());

		::free(mallocPtr);
	}

	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = animationCount;

		ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(texture.Get(), &desc, srv.GetAddressOf());
	}
}

void AnimationGroup::Bind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->VSSetShaderResources(10, 1, this->srv.GetAddressOf());
}

void AnimationGroup::CreateAnimationTransform(unsigned int idx)
{
	auto animation = this->animationVec[idx];

	auto& animationClip = animation->GetAnimationClip();
	auto& boneMap = ResourceManager::Instance.Get().GetFBXBoneData(std::string{ fbxName.begin(), fbxName.end() });
	auto fbxNode = ResourceManager::Instance.Get().GetFBXNode(fbxName);
	for (int f = 0; f < animationClip.totalFrame; ++f)
	{
		ReadBone(fbxNode, DirectX::SimpleMath::Matrix::Identity, animationClip, idx, f);
	}
}

void AnimationGroup::ReadBone(FBXNode* fbxNode, DirectX::SimpleMath::Matrix parentMatrix, AnimationClip& clip, int animIndex, int frame)
{
	auto& boneInfoMap = ResourceManager::Instance.Get().GetFBXBoneData(std::string{ fbxName.begin(), fbxName.end() });

	DirectX::SimpleMath::Matrix srt = fbxNode->transformMatrix;

	auto iter = boneInfoMap.find(fbxNode->nodeName);

	if (iter != boneInfoMap.end())
	{
		DirectX::SimpleMath::Matrix T = DirectX::SimpleMath::Matrix::CreateTranslation(clip.keyFrameInfoVec[iter->second.index][frame].pos);
		DirectX::SimpleMath::Matrix R = DirectX::SimpleMath::Matrix::CreateFromQuaternion(clip.keyFrameInfoVec[iter->second.index][frame].rot);
		DirectX::SimpleMath::Matrix S = DirectX::SimpleMath::Matrix::CreateScale(clip.keyFrameInfoVec[iter->second.index][frame].scale);

		srt = S * R * T;

		this->animTransformVec[animIndex].transforms[frame][iter->second.index] = iter->second.offset * srt * parentMatrix;
	}

	for (int i = 0; i < fbxNode->child.size(); ++i)
	{
		ReadBone(fbxNode->child[i], srt * parentMatrix, clip, animIndex, frame);
	}
}
