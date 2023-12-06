#pragma once
#include "IShader.h"

#include "Resource.h"

#include <assert.h>
#include <wrl.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <D3Dcommon.h>





class Shader : public yunuGI::IShader, public Resource
{
public:
	virtual void CreateShader(const std::wstring& shaderPath) = 0;
	virtual void Bind() = 0;
	virtual void UnBind() = 0;
	virtual const yunuGI::ShaderInfo& GetShaderInfo() const { return this->shaderInfo; }
	

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> errorMSG;
	
};
