#pragma once

#include <wrl.h>

#include <dxgi1_2.h>
#include <d3d11.h>

class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	void Init(HWND hWnd, int width, int height);
	void Resize(int width, int height);
	void Release();

#pragma region Getter
	Microsoft::WRL::ComPtr<IDXGISwapChain1>& GetSwapChain() { return swapChain; }
	D3D11_VIEWPORT& GetViewPort() { return vp; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV() { return RTV; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV() { return DSV; }

#pragma endregion

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DSBuffer;

	D3D11_VIEWPORT vp;

	unsigned int backBufferIndex;
};

