#pragma once

#include <assert.h>
#include <string>
#include <wrl.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <D3Dcommon.h>

class Shader
{
public:
	virtual void CreateShader(const std::wstring& shaderPath) abstract;
	virtual void Bind() abstract;
	virtual void UnBind() abstract;

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> errorMSG;
	
};
