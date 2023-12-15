#include "VertexShader.h"

#include <vector>

#include <d3d11shader.h>

#include "ResourceBuilder.h"
#include "Device.h"

void VertexShader::CreateShader(const std::wstring& shaderPath)
{
	unsigned int _compileFlag = 0;
#ifdef _DEBUG
	_compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	::D3DReadFileToBlob(shaderPath.c_str(), vsBuffer.GetAddressOf());

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

	if (FAILED(ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateVertexShader(
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

void VertexShader::CreateInputLayout()
{
	//ID3D11ShaderReflection* _shaderReflection = nullptr;
	//::D3DReflect(
	//	vsBuffer->GetBufferPointer(),
	//	vsBuffer->GetBufferSize(),
	//	IID_ID3D11ShaderReflection,
	//	(void**)&_shaderReflection
	//);

	//D3D11_SHADER_DESC _shaderDesc;
	//_shaderReflection->GetDesc(&_shaderDesc);

	//std::vector<D3D11_INPUT_ELEMENT_DESC> _inputLayoutDescVec;

	//for (int i = 0; i < _shaderDesc.InputParameters; i++)
	//{
	//	D3D11_INPUT_ELEMENT_DESC _inputDesc;
	//	_inputLayoutDescVec.emplace_back(_inputDesc);

	//	D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
	//	_shaderReflection->GetInputParameterDesc(i, &paramDesc);

	//	_inputLayoutDescVec[i].SemanticName = paramDesc.SemanticName;
	//	_inputLayoutDescVec[i].SemanticIndex = paramDesc.SemanticIndex;
	//	_inputLayoutDescVec[i].InputSlot = 0;
	//	_inputLayoutDescVec[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//	_inputLayoutDescVec[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//	_inputLayoutDescVec[i].InstanceDataStepRate = 0;

	//	if (paramDesc.Mask == 1)
	//	{
	//		if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32_UINT;
	//		}
	//		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32_SINT;
	//		}
	//		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32_FLOAT;
	//		}
	//	}
	//	else if (paramDesc.Mask <= 3) 
	//	{
	//		if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) 
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32_UINT;
	//		}
	//		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) 
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32_SINT;
	//		}
	//		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) 
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32_FLOAT;
	//		}
	//	}
	//	else if (paramDesc.Mask <= 7) 
	//	{
	//		if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32_UINT;
	//		}
	//		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) 
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32_SINT;
	//		}
	//		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) 
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//		}
	//	}
	//	else if (paramDesc.Mask <= 15) 
	//	{
	//		if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) 
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32A32_UINT;
	//		}
	//		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	//		}
	//		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
	//		{
	//			_inputLayoutDescVec[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//		}
	//	}
	//}



	//ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateInputLayout(&_inputLayoutDescVec[0], _inputLayoutDescVec.size(),
	//	this->vsBuffer->GetBufferPointer(), this->vsBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());


	//float3 pos : POSITION;
	//float4 color : COLOR;
	//float2 uv : TEXCOORD;
	//float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	//uint4 indices : BLENDINDICES;
	//float4 weight : BLENDWEIGHT;

	D3D11_INPUT_ELEMENT_DESC _inputLayoutDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BLENDINDICES", 0,  DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BLENDWEIGHT", 0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	unsigned int _numElements = ARRAYSIZE(_inputLayoutDesc);

	HRESULT hr = ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateInputLayout(_inputLayoutDesc, _numElements,
		this->vsBuffer->GetBufferPointer(), this->vsBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());

	int a = 1;
}
