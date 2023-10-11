#pragma once

#include "Shader.h"

class PixelShader : public Shader
{
public:
	virtual void CreateShader(const std::wstring& shaderPath) override;
	virtual void Bind() override;
	virtual void UnBind() override;

private:
	void CreateRasterizerState();
	void CreateShaderState(const std::wstring& shaderPath);
	void CreateRasterizerState(const std::string& fileContent);
	void CreateSamplerState();

private:
	Microsoft::WRL::ComPtr<ID3DBlob> psBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

