#pragma once

#include "Shader.h"



class VertexShader : public Shader
{
public:
	virtual void CreateShader(const std::wstring& shaderPath) override;
	virtual void Bind() override;
	virtual void UnBind() override;

private:
	void CreateInputLayout();

private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3DBlob> vsBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
};

