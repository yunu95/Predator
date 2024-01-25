#pragma once

#include <d3d11.h>

#include <vector>
#include <wrl.h>

#include "Struct.h"

class RenderTargetGroup
{
public:
	~RenderTargetGroup();
	void OMSetRenderTarget(bool isPBR = false);
	void UnBind();
	void Clear();

#pragma region Setter
	void SetRenderTargetVec(std::vector<RenderTarget>& rtVec, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = nullptr);
#pragma endregion

#pragma region Getter
	std::shared_ptr<Texture>& GetRTTexture(int index);
#pragma endregion 


private:
	std::vector<RenderTarget> rtVec;

	std::vector<ID3D11RenderTargetView*> RTVVec;

	// UnBind용 null Render Target
	std::vector<ID3D11RenderTargetView*> nullRTV;

	unsigned int rtCount;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
};

