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


	if (FAILED(ResourceBuilder::Instance.Get().device->GetDevice()->CreatePixelShader(
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

	CreateShaderState(hlslPath);
}

void PixelShader::Bind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetSamplers(this->samplerSlot, 1, this->samplerState.GetAddressOf());

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetState(this->rasterizerState.Get());

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetDepthStencilState(this->depthStencilState.Get(), stencilRef);

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShader(ps.Get(), nullptr, 0);

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetBlendState(this->blendState.Get(), nullptr, 0xFFFFFFFF);
}

void PixelShader::UnBind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
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

	CreateShaderType(fileContent);
	CreateRasterizerState(fileContent);
	CreateDepthStencilState(fileContent);
	CreateBlendState(fileContent);
	CreateSamplerState(fileContent);
}

void PixelShader::CreateShaderType(const std::string& fileContent)
{
	std::regex commentRegex("// ShaderType : (\\w+)");
	std::smatch matches;
	if (std::regex_search(fileContent, matches, commentRegex))
	{
		if (matches.size() >= 2)
		{
			std::string shaderType = matches[1].str();
			if (shaderType == "Forward")
			{
				this->shaderInfo.shaderType = yunuGI::ShaderType::Forward;
			}
			else if (shaderType == "Deferred")
			{
				this->shaderInfo.shaderType = yunuGI::ShaderType::Deferred;
			}
			else if (shaderType == "Light")
			{
				this->shaderInfo.shaderType = yunuGI::ShaderType::Light;
			}
			else if (shaderType == "Particle")
			{
				this->shaderInfo.shaderType = yunuGI::ShaderType::Particle;
			}
			else if (shaderType == "Decal")
			{
				this->shaderInfo.shaderType = yunuGI::ShaderType::Decal;
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
}

void PixelShader::CreateRasterizerState(const std::string& fileContent)
{
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.f;

	std::regex commentRegex("// RasterType : (\\w+)");
	std::smatch matches;
	if (std::regex_search(fileContent, matches,commentRegex))
	{
		if (matches.size() >= 2)
		{
			std::string shaderType = matches[1].str();
			if (shaderType == "Solid")
			{
				this->shaderInfo.rasterizer = yunuGI::Rasterizer::Solid;
				rasterDesc.FillMode = D3D11_FILL_SOLID;
			}
			else if (shaderType == "Wireframe")
			{
				this->shaderInfo.rasterizer = yunuGI::Rasterizer::Wire;
				rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
			}
			else if (shaderType == "Light")
			{
				this->shaderInfo.rasterizer = yunuGI::Rasterizer::Solid;
				rasterDesc.FillMode = D3D11_FILL_SOLID;
				rasterDesc.DepthClipEnable = false;
			}
			else if (shaderType == "Shadow")
			{
				rasterDesc.DepthBias = 16777;
				rasterDesc.DepthBiasClamp = 0.f;
				rasterDesc.DepthClipEnable = true;
				rasterDesc.SlopeScaledDepthBias = 1.f;

				this->shaderInfo.rasterizer = yunuGI::Rasterizer::Solid;
				rasterDesc.FillMode = D3D11_FILL_SOLID;
			}
			else
			{
				// 뒤에 입력이 없었으니 assert
				assert(FALSE);
			}
		}
		else
		{
			// RasterType : 이라는 문자열을 hlsl파일 안에 정의하지 않았으니 assert
			assert(FALSE);
		}
	}

	commentRegex = std::regex("// CullType : (\\w+)");
	if (std::regex_search(fileContent, matches, commentRegex))
	{
		if (matches.size() >= 2)
		{
			std::string shaderType = matches[1].str();
			if (shaderType == "CullBack")
			{
				rasterDesc.CullMode = D3D11_CULL_BACK;
			}
			else if (shaderType == "CullFront")
			{
				rasterDesc.CullMode = D3D11_CULL_FRONT;
			}
			else if (shaderType == "CullNone")
			{
				rasterDesc.CullMode = D3D11_CULL_NONE;
			}
			else
			{
				// 뒤에 입력이 없었으니 assert
				assert(FALSE);
			}
		}
		else
		{
			// CullType : 이라는 문자열을 hlsl파일 안에 정의하지 않았으니 assert
			assert(FALSE);
		}
	}

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateRasterizerState(&rasterDesc, this->rasterizerState.GetAddressOf());
}

void PixelShader::CreateDepthStencilState(const std::string& fileContent)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	
	depthStencilDesc.DepthEnable = true; // 깊이 버퍼 사용 여부
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; // 깊이 값을 비교하는 함수
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 깊이 값을 쓰는지 여부

	depthStencilDesc.StencilEnable = true; // 스텐실 버퍼 사용 여부
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK; // 스텐실 값을 읽을 때의 마스크
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK; // 스텐실 값을 쓸 때의 마스크

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트에 실패하면 수행할 동작
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트는 성공했으나 깊이 테스트는 실패한 경우에 수행할 동작
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; // 스텐실 테스트와 깊이 테스트 모두 성공한 경우에 수행할 동작
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 스텐실 테스트에 사용할 함수

	

	std::regex commentRegex("// DepthType : (\\w+)");
	std::smatch matches;
	if (std::regex_search(fileContent, matches, commentRegex))
	{
		if (matches.size() >= 2)
		{
			std::string shaderType = matches[1].str();
			if (shaderType == "Less")
			{
				depthStencilDesc.DepthEnable = true; 
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; 
			}
			else if (shaderType == "LessEqual")
			{
				depthStencilDesc.DepthEnable = true; 
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; 
			}
			else if (shaderType == "Greater")
			{
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
			}
			else if (shaderType == "GreaterEqual")
			{
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			}
			else if (shaderType == "NoDepthTest")
			{
				depthStencilDesc.DepthEnable = false;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
			}
			else if (shaderType == "NoDepthTestNoWrite")
			{
				depthStencilDesc.DepthEnable = false;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			}
			else if (shaderType == "LessNoWrite")
			{
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			}
			else
			{
				// 뒤에 입력이 없었으니 assert
				assert(FALSE);
			}
		}
		else
		{
			// DepthType : 이라는 문자열을 hlsl파일 안에 정의하지 않았으니 assert
			assert(FALSE);
		}
	}

	commentRegex = std::regex("// StencilType : (\\w+)");
	if (std::regex_search(fileContent, matches, commentRegex))
	{
		if (matches.size() >= 2)
		{
			std::string shaderType = matches[1].str();
			if (shaderType == "Character")
			{
				this->stencilRef = 2;
				//depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트에 실패하면 수행할 동작
				//depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR; // 스텐실 테스트는 성공했으나 깊이 테스트는 실패한 경우에 수행할 동작
				//depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트와 깊이 테스트 모두 성공한 경우에 수행할 동작
				//depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 스텐실 테스트에 사용할 함수
			}
			else if (shaderType == "Silhouette")
			{
				this->stencilRef = 1;
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트에 실패하면 수행할 동작
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트는 성공했으나 깊이 테스트는 실패한 경우에 수행할 동작
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트와 깊이 테스트 모두 성공한 경우에 수행할 동작
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL; // 스텐실 테스트에 사용할 함수
			}
		}
		else
		{
			//assert(FALSE);
		}
	}
	depthStencilDesc.BackFace = depthStencilDesc.FrontFace; // 양면 스텐실을 

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateDepthStencilState(&depthStencilDesc, this->depthStencilState.GetAddressOf());
}

void PixelShader::CreateBlendState(const std::string& fileContent)
{
	D3D11_BLEND_DESC blendDesc{};

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	std::regex commentRegex("// BlendType : (\\w+)");
	std::smatch matches;
	if (std::regex_search(fileContent, matches, commentRegex))
	{
		if (matches.size() >= 2)
		{
			std::string shaderType = matches[1].str();
			if (shaderType == "Default")
			{

			}
			else if (shaderType == "Light")
			{
				blendDesc.RenderTarget[0].BlendEnable = TRUE;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			}
			else if (shaderType == "AlphaBlend")
			{
				blendDesc.RenderTarget[0].BlendEnable = TRUE; // 블렌딩을 활성화
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // 소스 색상의 알파 값 사용
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 대상 색상의 알파 값 사용
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 블렌딩 연산: 소스 색상 * 소스 알파 + (1 - 소스 알파) * 대상 색상
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			}
			else if (shaderType == "Floor")
			{
				blendDesc.RenderTarget[0].BlendEnable = TRUE;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			}
			else
			{
				// 뒤에 입력이 없으니 assert
				assert(FALSE);
			}
		}
		else
		{
			// BlendType : 이라는 문자열을 hlsl파일 안에 정의하지 않았으니 assert
			assert(FALSE);
		}
	}
	ResourceBuilder::Instance.Get().device->GetDevice()->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
}

void PixelShader::CreateSamplerState(const std::string& fileContent)
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	//samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	std::regex commentRegex("// Sampler : (\\w+)");
	std::smatch matches;
	if (std::regex_search(fileContent, matches, commentRegex))
	{
		if (matches.size() >= 2)
		{
			std::string shaderType = matches[1].str();
			if (shaderType == "Default")
			{

			}
			else if (shaderType == "Shadow")
			{
				samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

				float borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				samplerDesc.BorderColor[0] = borderColor[0];
				samplerDesc.BorderColor[1] = borderColor[1];
				samplerDesc.BorderColor[2] = borderColor[2];
				samplerDesc.BorderColor[3] = borderColor[3];

				samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

				samplerDesc.MipLODBias = 0.0f;

				samplerDesc.MaxAnisotropy = 1;

				this->samplerSlot = 1;
			}
			else
			{
				// 뒤에 입력이 없으니 assert
				assert(FALSE);
			}
		}
		else
		{
			// Sampler : 이라는 문자열을 hlsl파일 안에 정의하지 않았으니 assert
			assert(FALSE);
		}
	}

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());
}