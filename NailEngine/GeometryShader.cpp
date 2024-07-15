#include "GeometryShader.h"

#include "ResourceBuilder.h"
#include "Device.h"

void GeometryShader::CreateShader(const std::wstring& shaderPath)
{
	unsigned int _compileFlag = 0;
#ifdef _DEBUG
	_compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	::D3DReadFileToBlob(shaderPath.c_str(), gsBuffer.GetAddressOf());


	if (FAILED(ResourceBuilder::Instance.Get().device->GetDevice()->CreateGeometryShader(
		gsBuffer->GetBufferPointer(),
		gsBuffer->GetBufferSize(),
		nullptr,
		gs.GetAddressOf())))
	{
		::MessageBoxA(nullptr, "Failed to create geometry shader!", nullptr, MB_OK);
		assert(true);
	}
}

void GeometryShader::Bind() const 
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->GSSetShader(gs.Get(), nullptr, 0);
}

void GeometryShader::UnBind() const
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
}