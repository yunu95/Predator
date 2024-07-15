#include "VertexShader.h"

#include <vector>

#include <d3d11shader.h>

#include "ResourceBuilder.h"
#include "Device.h"

#include <Windows.h>
#include <string>

std::wstring ConvertToWideString(const char* str) {
	int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	wchar_t* buffer = new wchar_t[size];
	MultiByteToWideChar(CP_ACP, 0, str, -1, buffer, size);
	std::wstring result(buffer);
	delete[] buffer;
	return result;
}

void VertexShader::CreateShader(const std::wstring& shaderPath)
{
	unsigned int _compileFlag = 0;
#ifdef _DEBUG
	_compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = ::D3DReadFileToBlob(shaderPath.c_str(), vsBuffer.GetAddressOf());
	//if (FAILED(::D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	//	, "main", "vs_5_0", _compileFlag, NULL, vsBuffer.GetAddressOf(), errorMSG.GetAddressOf())))
	//{
	//	if (errorMSG)
	//	{
	//		::MessageBoxA(nullptr, "Failed to compile shader!", nullptr, MB_OK);
	//		assert(true);
	//	}
	//	else
	//	{
	//		::MessageBoxA(nullptr, "Shader path not found!", nullptr, MB_OK);
	//		assert(true);
	//	}
	//}

	if (FAILED(ResourceBuilder::Instance.Get().device->GetDevice()->CreateVertexShader(
		vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(),
		nullptr,
		vs.GetAddressOf())))
	{
		::MessageBoxA(nullptr, "Failed to create vertex shader!", nullptr, MB_OK);
		assert(true);
	}

	CreateInputLayout();
}

void VertexShader::Bind() const 
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetInputLayout(this->inputLayout.Get());
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->VSSetShader(vs.Get(), nullptr, 0);
}

void VertexShader::UnBind() const
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetInputLayout(nullptr);
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
}

void VertexShader::CreateInputLayout()
{
	ID3D11ShaderReflection* _shaderReflection = nullptr;
	::D3DReflect(
		vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)&_shaderReflection
	);

	D3D11_SHADER_DESC _shaderDesc;
	_shaderReflection->GetDesc(&_shaderDesc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> _inputLayoutDescVec;

	for (int i = 0; i < _shaderDesc.InputParameters; i++)
	{
		D3D11_INPUT_ELEMENT_DESC _inputDesc;
		_inputLayoutDescVec.emplace_back(_inputDesc);

		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		_shaderReflection->GetInputParameterDesc(i, &paramDesc);

		_inputLayoutDescVec[i].SemanticName = paramDesc.SemanticName;
		_inputLayoutDescVec[i].SemanticIndex = paramDesc.SemanticIndex;
		_inputLayoutDescVec[i].InputSlot = 0;
		_inputLayoutDescVec[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		_inputLayoutDescVec[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		_inputLayoutDescVec[i].InstanceDataStepRate = 0;

		if (ConvertToWideString(paramDesc.SemanticName) == L"INST")
		{
			_inputLayoutDescVec[i].InputSlot = 1;
			_inputLayoutDescVec[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			_inputLayoutDescVec[i].InstanceDataStepRate = 1;
		}

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32_UINT;
			}
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32_SINT;
			}
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32_FLOAT;
			}
		}
		else if (paramDesc.Mask <= 3) 
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) 
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32_UINT;
			}
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) 
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32_SINT;
			}
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) 
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32_FLOAT;
			}
		}
		else if (paramDesc.Mask <= 7) 
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32_UINT;
			}
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) 
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32_SINT;
			}
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) 
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
		}
		else if (paramDesc.Mask <= 15) 
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) 
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32A32_UINT;
			}
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32A32_SINT;
			}
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}
	}

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateInputLayout(&_inputLayoutDescVec[0], _inputLayoutDescVec.size(),
		this->vsBuffer->GetBufferPointer(), this->vsBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());
}
