#pragma once
#include "Shader.h"



class GeometryShader : public Shader
{
public:
	virtual void CreateShader(const std::wstring& shaderPath) override;
	virtual void Bind() override;
	virtual void UnBind() override;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> gsBuffer;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs;
};

