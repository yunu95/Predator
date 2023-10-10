#include "PixelShader.h"

#include <regex>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <assert.h>

#include <d3d11shader.h>

#include "ResourceBuilder.h"
#include "Device.h"

void PixelShader::CreateShader(const std::wstring& shaderPath)
{
	unsigned int _compileFlag = 0;
#ifdef _DEBUG
	_compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	::D3DReadFileToBlob(shaderPath.c_str(), psBuffer.GetAddressOf());

	//if (FAILED(::D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	//	, "main", "ps_5_0", _compileFlag, NULL, psBuffer.GetAddressOf(), errorMSG.GetAddressOf())))
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

	if (FAILED(ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreatePixelShader(
		psBuffer->GetBufferPointer(),
		psBuffer->GetBufferSize(),
		nullptr,
		ps.GetAddressOf())))
	{
		::MessageBoxA(nullptr, "Failed to create pixel shader!", nullptr, MB_OK);
		assert(true);
	}

	std::filesystem::path filePath(shaderPath);
	std::wstring fileName = filePath.stem().wstring();
	std::wstring hlslPath{L"Shader/"};
	hlslPath = hlslPath + fileName + L".hlsl";

	CreateRasterizerState();
	CreateShaderState(hlslPath);
}

void PixelShader::Bind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->RSSetState(this->rasterizerState.Get());

	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->PSSetShader(ps.Get(), nullptr, 0);
}

void PixelShader::UnBind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->PSSetShader(nullptr, nullptr, 0);
}

void PixelShader::CreateRasterizerState()
{
	ID3D11ShaderReflection* pShaderReflection = nullptr;

	D3DReflect(
		psBuffer->GetBufferPointer(),
		psBuffer->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)&pShaderReflection
	);

	ID3D11ShaderReflectionVariable* pVariable = pShaderReflection->GetVariableByName("A");

	if (pVariable) {
		D3D11_SHADER_VARIABLE_DESC varDesc;
		HRESULT hr = pVariable->GetDesc(&varDesc);
	}
}

void PixelShader::CreateShaderState(const std::wstring& shaderPath)
{
	std::ifstream file(shaderPath);

	std::string fileContent((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

	std::regex commentRegex("//.*?ShaderInfo");
	if (!std::regex_search(fileContent, commentRegex))
	{
		// hlsl안에 ShaderInfo라는 주석이 없기에 assert
		assert(FALSE);
	}

	CreateRasterizerState(fileContent);
}

void PixelShader::CreateRasterizerState(const std::string& fileContent)
{
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.f;


	std::regex commentRegex("// ShaderType : (\\w+)");
	std::smatch matches;
	if (std::regex_search(fileContent, matches,commentRegex))
	{
		if (matches.size() >= 2)
		{
			std::string shaderType = matches[1].str();
			if (shaderType == "Solid")
			{
				rasterDesc.FillMode = D3D11_FILL_SOLID;
			}
			else if (shaderType == "Wireframe")
			{
				rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
			}
			else
			{
				// 뒤에 입력이 없었으니 assert
				assert(FALSE);
			}
		}
		else
		{
			// ShaderType : 이라는 문자열을 hlsl파일 안에 정의하지 않았으니 assert
			assert(FALSE);
		}
	}

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateRasterizerState(&rasterDesc, this->rasterizerState.GetAddressOf());
}