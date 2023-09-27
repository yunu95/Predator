#include "VertexShader.h"

#include "ResourceBuilder.h"
#include "Device.h"

void VertexShader::CreateShader(const std::wstring& shaderPath)
{
	unsigned int _compileFlag = 0;
#ifdef _DEBUG
	_compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, "main", "vs_5_0", _compileFlag, NULL, vsBuffer.GetAddressOf(), errorMSG.GetAddressOf())))
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

	if (FAILED(ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateVertexShader(
		vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(),
		nullptr,
		vs.GetAddressOf())))
	{
		::MessageBoxA(nullptr, "Failed to create vertex shader!", nullptr, MB_OK);
		assert(true);
	}

	D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int _numElements = ARRAYSIZE(_inputLayoutDesc);

	ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateInputLayout(_inputLayoutDesc, _numElements,
		this->vsBuffer->GetBufferPointer(), this->vsBuffer->GetBufferSize(), inputLayout.GetAddressOf());
}

void VertexShader::Bind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetInputLayout(this->inputLayout.Get());
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->VSSetShader(vs.Get(), nullptr, 0);
}

void VertexShader::UnBind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetInputLayout(nullptr);
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->VSSetShader(nullptr, nullptr, 0);
}
