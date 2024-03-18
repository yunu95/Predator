#include "SkyBoxPass.h"

#include "Texture.h"
#include "mesh.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "CameraManager.h"
#include "NailCamera.h"
#include "NailEngine.h"
#include "ConstantBuffer.h"
#include "ResourceManager.h"
#include "RenderTargetGroup.h"
#include "ResourceBuilder.h"
#include "Device.h"

LazyObjects<SkyBoxPass> SkyBoxPass::Instance;

void SkyBoxPass::Init(Texture* texture, Mesh* mesh, yunuGI::IShader* vs, yunuGI::IShader* ps)
{
	this->texture = texture;
	this->mesh = mesh;
	this->vs = reinterpret_cast<VertexShader*>(vs);
	this->ps = reinterpret_cast<PixelShader*>(ps);

	//BuildIrradianceMap();
	//BuildSpecularMap();
	//BuildLUT();


	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MaxAnisotropy = (D3D11_FILTER_MIN_MAG_MIP_LINEAR == D3D11_FILTER_ANISOTROPIC) ? D3D11_REQ_MAXANISOTROPY : 1;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	
	ResourceBuilder::Instance.Get().device->GetDevice()->CreateSamplerState(&desc, &samplerState);
}

void SkyBoxPass::Render()
{
	/// Render SkyBox
	//DirectX::SimpleMath::Vector3 pos;
	//DirectX::SimpleMath::Vector3 scale;
	//DirectX::SimpleMath::Quaternion quat;
	//CameraManager::Instance.Get().GetMainCamera()->GetWTM().Decompose(scale, quat, pos);

	//// CB Set
	//MatrixBuffer matrixBuffer;
	//matrixBuffer.WTM = DirectX::SimpleMath::Matrix::CreateTranslation(pos);
	//matrixBuffer.VTM = CameraManager::Instance.Get().GetMainCamera()->GetVTM();
	//matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM();
	//matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	//matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//matrixBuffer.VTMInv = matrixBuffer.VTM.Invert();
	//NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));

	//// Bind Texture
	////this->texture->Bind(9);
	////ResourceManager::Instance.Get().GetTexture(L"PointLightShadowDepth")->Bind(9);
	////ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShaderResources(9, 1, this->irradianceSRV.GetAddressOf());
	////ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShaderResources(9, 1, this->prefilteredSRV.GetAddressOf());


	//// Bind Shader
	//this->vs->Bind();
	//this->ps->Bind();

	//// Mesh Render
	//this->mesh->Render();
	///
}

void SkyBoxPass::BuildIrradianceMap()
{
	ID3D11Texture2D* cubeTexture;
	D3D11_TEXTURE2D_DESC cubeTextureDesc = {};
	//cubeTextureDesc.Width = this->texture->GetWidth(); // 큐브맵의 한 면의 크기
	//cubeTextureDesc.Height = this->texture->GetHeight();
	cubeTextureDesc.Width = 128;
	cubeTextureDesc.Height = 128;
	cubeTextureDesc.ArraySize = 6; 
	cubeTextureDesc.MipLevels = 1;
	cubeTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
	cubeTextureDesc.SampleDesc.Count = 1;
	cubeTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	cubeTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	cubeTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


	ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&cubeTextureDesc, nullptr, &cubeTexture);



	//// 큐브맵 렌더 타겟 뷰 생성
	ID3D11RenderTargetView* renderTargetViewArray[6];
	for (UINT i = 0; i < 6; ++i) {
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		renderTargetViewDesc.Format = cubeTextureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		renderTargetViewDesc.Texture2DArray.MipSlice = 0;
		renderTargetViewDesc.Texture2DArray.ArraySize = 1;
		renderTargetViewDesc.Texture2DArray.FirstArraySlice = i;

		ResourceBuilder::Instance.Get().device->GetDevice()->CreateRenderTargetView(cubeTexture, &renderTargetViewDesc, &renderTargetViewArray[i]);
	}


	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; 
	for (UINT i = 0; i < 6; ++i) 
	{
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->ClearRenderTargetView(renderTargetViewArray[i], clearColor);
	}

	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	CameraManager::Instance.Get().GetMainCamera()->GetWTM().Decompose(scale, quat, pos);

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = 128;
	viewport.Height = 128;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	for (int i = 0; i < 6; ++i)
	{
		// 각 렌더 타겟에 대한 시야 및 방향 설정
		DirectX::SimpleMath::Vector3 targetPos;
		DirectX::SimpleMath::Vector3 upVec;
		switch (i)
		{
			case 0: // +X
				targetPos = pos + DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				break;
			case 1: // -X
				targetPos = pos + DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				break;
			case 2: // +Y
				targetPos = pos + DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
				break;
			case 3: // -Y
				targetPos = pos + DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
				break;
			case 4: // +Z
				targetPos = pos + DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				break;
			case 5: // -Z
				targetPos = pos + DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				break;
		}

		// 뷰 행렬 계산
		DirectX::SimpleMath::Matrix viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(pos, targetPos, upVec);

		MatrixBuffer matrixBuffer;
		matrixBuffer.WTM = DirectX::SimpleMath::Matrix::CreateTranslation(pos);
		matrixBuffer.VTM = viewMatrix;
		matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM90();
		matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
		NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1, &renderTargetViewArray[i], nullptr);

		// Bind Texture
		this->texture->Bind(9);

		// Bind Shader
		std::static_pointer_cast<VertexShader>(ResourceManager::Instance.Get().GetShader(L"IrradianceVS.cso"))->Bind();
		std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetShader(L"IrradiancePS.cso"))->Bind();


		// Mesh Render
		//ResourceManager::Instance.Get().GetMesh(L"Cube")->Render();
		this->mesh->Render();
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeTextureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = cubeTextureDesc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(cubeTexture, &srvDesc, &irradianceSRV);

	// 렌더링이 완료되면 렌더 타겟을 초기화
	ID3D11RenderTargetView* nullRenderTargetView = nullptr;
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1, &nullRenderTargetView, nullptr);
}

void SkyBoxPass::BuildSpecularMap()
{
	ID3D11Texture2D* cubeTexture;
	D3D11_TEXTURE2D_DESC cubeTextureDesc = {};
	//cubeTextureDesc.Width = this->texture->GetWidth(); // 큐브맵의 한 면의 크기
	//cubeTextureDesc.Height = this->texture->GetHeight();
	cubeTextureDesc.Width = 512;
	cubeTextureDesc.Height = 512;
	cubeTextureDesc.ArraySize = 6; // 큐브맵은 6개의 면으로 이루어집니다.
	cubeTextureDesc.MipLevels = 1;
	cubeTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 텍스처의 포맷을 설정하세요.
	cubeTextureDesc.SampleDesc.Count = 1;
	cubeTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	cubeTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	cubeTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&cubeTextureDesc, nullptr, &cubeTexture);



	// 큐브맵 렌더 타겟 뷰 생성
	ID3D11RenderTargetView* renderTargetViewArray[6];
	for (UINT i = 0; i < 6; ++i) {
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		renderTargetViewDesc.Format = cubeTextureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		renderTargetViewDesc.Texture2DArray.MipSlice = 0;
		renderTargetViewDesc.Texture2DArray.ArraySize = 1;
		renderTargetViewDesc.Texture2DArray.FirstArraySlice = i;

		ResourceBuilder::Instance.Get().device->GetDevice()->CreateRenderTargetView(cubeTexture, &renderTargetViewDesc, &renderTargetViewArray[i]);
	}

	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	CameraManager::Instance.Get().GetMainCamera()->GetWTM().Decompose(scale, quat, pos);

	// CB Set


	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = 512;
	viewport.Height = 512;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	for (int i = 0; i < 6; ++i)
	{
		// 각 렌더 타겟에 대한 시야 및 방향 설정
		DirectX::SimpleMath::Vector3 targetPos;
		DirectX::SimpleMath::Vector3 upVec;
		switch (i)
		{
			case 0: // +X
				targetPos = pos + DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				break;
			case 1: // -X
				targetPos = pos + DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				break;
			case 2: // +Y
				targetPos = pos + DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
				break;
			case 3: // -Y
				targetPos = pos + DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
				break;
			case 4: // +Z
				targetPos = pos + DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				break;
			case 5: // -Z
				targetPos = pos + DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
				upVec = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
				break;
		}

		// 뷰 행렬 계산
		DirectX::SimpleMath::Matrix viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(pos, targetPos, upVec);

		MatrixBuffer matrixBuffer;
		matrixBuffer.WTM = DirectX::SimpleMath::Matrix::CreateTranslation(pos);
		matrixBuffer.VTM = viewMatrix;
		matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM90();
		matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
		matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
		matrixBuffer.VTMInv = matrixBuffer.VTM.Invert();
		NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1, &renderTargetViewArray[i], nullptr);

		// Bind Texture
		this->texture->Bind(9);

		// Bind Shader
		std::static_pointer_cast<VertexShader>(ResourceManager::Instance.Get().GetShader(L"PreFilteredVS.cso"))->Bind();
		std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetShader(L"PreFilteredPS.cso"))->Bind();


		// Mesh Render
		this->mesh->Render();
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeTextureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = cubeTextureDesc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	auto hr = ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(cubeTexture, &srvDesc, prefilteredSRV.GetAddressOf());

	// 렌더링이 완료되면 렌더 타겟을 초기화
	ID3D11RenderTargetView* nullRenderTargetView = nullptr;
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1, &nullRenderTargetView, nullptr);
}

void SkyBoxPass::BuildLUT()
{
	ID3D11Texture2D* cubeTexture;
	D3D11_TEXTURE2D_DESC cubeTextureDesc = {};
	//cubeTextureDesc.Width = this->texture->GetWidth(); // 큐브맵의 한 면의 크기
	//cubeTextureDesc.Height = this->texture->GetHeight();
	cubeTextureDesc.Width = 256;
	cubeTextureDesc.Height = 256;
	cubeTextureDesc.ArraySize = 1; // 큐브맵은 6개의 면으로 이루어집니다.
	cubeTextureDesc.MipLevels = 1;
	cubeTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 텍스처의 포맷을 설정하세요.
	cubeTextureDesc.SampleDesc.Count = 1;
	cubeTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	cubeTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	cubeTextureDesc.MiscFlags = 0;

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&cubeTextureDesc, nullptr, &cubeTexture);



	// 큐브맵 렌더 타겟 뷰 생성
	ID3D11RenderTargetView* renderTargetViewArray;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
	renderTargetViewDesc.Format = cubeTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2DArray.MipSlice = 0;
	renderTargetViewDesc.Texture2DArray.ArraySize = 1;

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateRenderTargetView(cubeTexture, &renderTargetViewDesc, &renderTargetViewArray);

	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	CameraManager::Instance.Get().GetMainCamera()->GetWTM().Decompose(scale, quat, pos);

	// CB Set


	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(cubeTextureDesc.Width);
	viewport.Height = static_cast<float>(cubeTextureDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;



	MatrixBuffer matrixBuffer;
	matrixBuffer.WTM = DirectX::SimpleMath::Matrix::CreateTranslation(pos);
	matrixBuffer.VTM = CameraManager::Instance.Get().GetMainCamera()->GetVTM();
	matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM();
	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	matrixBuffer.VTMInv = matrixBuffer.VTM.Invert();
	NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1, &renderTargetViewArray, nullptr);

	// Bind Texture
	this->texture->Bind(9);

	// Bind Shader
	std::static_pointer_cast<VertexShader>(ResourceManager::Instance.Get().GetShader(L"SpecLUTVS.cso"))->Bind();
	std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetShader(L"SpecLUTPS.cso"))->Bind();


	// Mesh Render
	ResourceManager::Instance.Get().GetMesh(L"Rectangle")->Render();

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeTextureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.TextureCube.MipLevels = cubeTextureDesc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(cubeTexture, &srvDesc, lutSRV.GetAddressOf());

	// 렌더링이 완료되면 렌더 타겟을 초기화
	ID3D11RenderTargetView* nullRenderTargetView = nullptr;
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1, &nullRenderTargetView, nullptr);
}

void SkyBoxPass::BindIBLTexture()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetSamplers(2,1, samplerState.GetAddressOf());

	this->texture->Bind(11);
	ResourceManager::Instance.Get().GetTexture(L"Texture/asdDiffuseHDR.dds")->Bind(12);
	//ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShaderResources(10, 1, this->irradianceSRV.GetAddressOf());
	ResourceManager::Instance.Get().GetTexture(L"Texture/asdSpecularHDR.dds")->Bind(13);
	//ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShaderResources(11, 1, this->prefilteredSRV.GetAddressOf());
	ResourceManager::Instance.Get().GetTexture(L"Texture/asdBrdf.dds")->Bind(14);
	//ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShaderResources(12, 1, this->lutSRV.GetAddressOf());
}
