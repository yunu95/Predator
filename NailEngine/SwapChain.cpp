#include "SwapChain.h"

#include <Windows.h>
#include <assert.h>

#include "ResourceBuilder.h"
#include "Device.h"

SwapChain::SwapChain()
	:swapChain{},
	RTV{},
	DSV{},
	DSBuffer{},
	vp{}
{

}

SwapChain::~SwapChain()
{
	swapChain->Release();
	RTV->Release();
	DSV->Release();
	DSBuffer->Release();
}

void SwapChain::Init(HWND hWnd, int width, int height)
{
	HRESULT _hr = S_FALSE;

	// 스왑체인
	DXGI_SWAP_CHAIN_DESC1 _sd{};
	_sd.Width = width;
	_sd.Height = height;
	_sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	_sd.Stereo = FALSE;
	_sd.SampleDesc.Count = 1;
	_sd.SampleDesc.Quality = 0;
	_sd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	_sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
	_sd.Scaling = DXGI_SCALING_STRETCH;
	_sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	_sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	_sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	IDXGIDevice2* _dxgiDevice;
	_hr = ResourceBuilder::Instance.Get().device->GetDevice()->QueryInterface(
		__uuidof(IDXGIDevice2), reinterpret_cast<void**>(&_dxgiDevice));

	assert(_hr == S_OK);

	Microsoft::WRL::ComPtr<IDXGIAdapter> _dxgiAdapter;
	_hr = _dxgiDevice->GetAdapter(&_dxgiAdapter);
	assert(_hr == S_OK);

	Microsoft::WRL::ComPtr<IDXGIFactory2> _dxgiFactory;
	_hr = _dxgiAdapter->GetParent(
		__uuidof(IDXGIFactory2),
		&_dxgiFactory
	);
	assert(_hr == S_OK);

	_hr = _dxgiFactory->CreateSwapChainForHwnd(
		ResourceBuilder::Instance.Get().device->GetDevice().Get(),
		hWnd,
		&_sd,
		0,
		nullptr,
		this->swapChain.GetAddressOf());

	_hr = _dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	assert(_hr == S_OK);

	// RTV 생성
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	this->swapChain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
	ResourceBuilder::Instance.Get().device->GetDevice()->CreateRenderTargetView(backBuffer.Get(), nullptr, this->RTV.GetAddressOf());

	// DSV 생성
	D3D11_TEXTURE2D_DESC _dsd{};
	_dsd.Width = width;
	_dsd.Height = height;
	_dsd.MipLevels = 1;
	_dsd.ArraySize = 1;
	_dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	_dsd.SampleDesc.Count = 1;
	_dsd.SampleDesc.Quality = 0;
	_dsd.Usage = D3D11_USAGE_DEFAULT;
	_dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	_dsd.CPUAccessFlags = 0;
	_dsd.MiscFlags = 0;

	_hr = ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(
		&_dsd,
		nullptr,
		this->DSBuffer.GetAddressOf());
	assert(_hr == S_OK);

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateDepthStencilView(
		this->DSBuffer.Get(),
		0,
		this->DSV.GetAddressOf()
	);

	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;

	_dxgiDevice->Release();
	_dxgiAdapter->Release();
	_dxgiFactory->Release();
}

void SwapChain::Release()
{
	swapChain->Release();
	RTV->Release();
	DSV->Release();
	DSBuffer->Release();
}

void SwapChain::SwapBackBufferIndex()
{
	this->backBufferIndex = (this->backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}
