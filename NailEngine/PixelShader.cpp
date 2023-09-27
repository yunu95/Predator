#include "PixelShader.h"

#include "ResourceBuilder.h"
#include "Device.h"

void PixelShader::CreateShader(const std::wstring& shaderPath)
{
	unsigned int _compileFlag = 0;
#ifdef _DEBUG
	_compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, "main", "ps_5_0", _compileFlag, NULL, psBuffer.GetAddressOf(), errorMSG.GetAddressOf())))
	{
		if (errorMSG)
		{
			::MessageBoxA(nullptr, "Failed to compile shader!", nullptr, MB_OK);
			assert(true);
		}
		else
		{
			::MessageBoxA(nullptr, "Shader path not found!", nullptr, MB_OK);
			assert(true);
		}
	}

	if (FAILED(ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreatePixelShader(
		psBuffer->GetBufferPointer(),
		psBuffer->GetBufferSize(),
		nullptr,
		ps.GetAddressOf())))
	{
		::MessageBoxA(nullptr, "Failed to create pixel shader!", nullptr, MB_OK);
		assert(true);
	}
}

void PixelShader::Bind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->PSSetShader(ps.Get(), nullptr, 0);
}

void PixelShader::UnBind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->PSSetShader(nullptr, nullptr, 0);
}
