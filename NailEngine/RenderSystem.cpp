#include "RenderSystem.h"

#include "NailEngine.h"

#include "ConstantBuffer.h"

#include "ResourceManager.h"
#include "Mesh.h"
#include "Animation.h"
#include "AnimationGroup.h"

#include "CameraManager.h"
#include "NailCamera.h"
#include "IRenderable.h"
#include "SKinnedMesh.h"
#include "ParticleSystem.h"

#include "ILight.h"
#include "LightManager.h"
#include "PointLight.h"
#include "DirectionalLight.h"

#include "NailCamera.h"
#include "RenderTargetGroup.h"

#include "Utils.h"

#include "ResourceBuilder.h"
#include "SwapChain.h"
#include "Device.h"

#include "NailAnimatorManager.h"
#include "NailAnimator.h"

#include "InstancingManager.h"

#include "UIImage.h"
#include "UIText.h"
#include "Texture.h"


#include "ShadowPass.h"
#include "PointLightShadowPass.h"
#include "SkyBoxPass.h"
#include "BloomPass.h"

#include "StaticMesh.h"

#include <iostream>
#include <fstream>

LazyObjects<RenderSystem> RenderSystem::Instance;

void RenderSystem::Finalize()
{
	for (auto& i : this->brushMap)
	{
		i.second->Release();
	}
	for (auto& i : this->wFormatMap)
	{
		i.second->Release();
	}
	wFactory->Release();
	d2dRT->Release();
	surface->Release();
	d2dFactory->Release();
}

void RenderSystem::Init()
{
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(ResourceBuilder::Instance.Get().device->GetDeviceContext().Get());
	commonStates = std::make_unique<DirectX::CommonStates>(ResourceBuilder::Instance.Get().device->GetDevice().Get());

	auto& shaderList = ResourceManager::Instance.Get().GetShaderList();
	for (auto& i : shaderList)
	{
		if (i->GetName() == L"TestPS.cso")
		{
			this->ps = i;
		}
		if (i->GetName() == L"TestVS.cso")
		{
			this->vs = i;
		}
	}

	CreateD2D();
}

void RenderSystem::CreateD2D()
{
	auto options = D2D1_FACTORY_OPTIONS();
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	HRESULT result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, d2dFactory.GetAddressOf());

	if (SUCCEEDED(result)) {
		result = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(this->wFactory.GetAddressOf()));

		if (FAILED(result)) PostQuitMessage(0);
	}

	ResourceBuilder::Instance.Get().swapChain->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(surface.GetAddressOf()));
	auto d2dRTProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0, 0);
	d2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &d2dRTProps, d2dRT.GetAddressOf());

	if (FAILED(result)) PostQuitMessage(0);
}

void RenderSystem::PushLightData()
{
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	LightParams params;
	params.lightCount = LightManager::Instance.Get().GetLightCount();

	unsigned int i = 0;

	for (auto& e : lightSet)
	{
		if (e->IsActive() == false)
		{
			i++;
			continue;
		}

		params.lights[i] = e->GetLightInfo();

		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Directional))
		{
			params.lights[i].direction = e->GetLightInfo().direction;
		}
		else if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Point))
		{
			params.lights[i].range = e->GetLightInfo().range;
			params.lights[i].farPlane = e->GetLightInfo().farPlane;
			params.lights[i].nearPlane = e->GetLightInfo().nearPlane;
		}
		else if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Spot))
		{
			params.lights[i].range = e->GetLightInfo().range;
			params.lights[i].angle = e->GetLightInfo().angle;
			params.lights[i].direction = e->GetLightInfo().direction;
		}
		i++;
	}

	NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::LIGHT))->PushGraphicsData(&params, sizeof(LightParams), static_cast<int>(CB_TYPE::LIGHT));
}

void RenderSystem::PushCameraData()
{
	CameraBuffer buffer;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	CameraManager::Instance.Get().GetMainCamera()->GetWTM().Decompose(scale, quat, mainCamPos);
	buffer.position = mainCamPos;
	NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::CAMERA))->PushGraphicsData(&buffer, sizeof(CameraBuffer), static_cast<int>(CB_TYPE::CAMERA));
}

void RenderSystem::Render()
{
	ResourceManager::Instance.Get().GetTexture(L"Texture/LightMap.dds")->Bind(24);

	UtilBuffer utilBuffer;
	utilBuffer.windowWidth = NailEngine::Instance.Get().GetWindowInfo().width;
	utilBuffer.windowHeight = NailEngine::Instance.Get().GetWindowInfo().height;
	utilBuffer.useIBL = NailEngine::Instance.Get().GetUseIBL();
	//utilBuffer.useLightMap = NailEngine::Instance.Get().GetUseLightMap();
	utilBuffer.useLightMap = true;
	NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::UTIL))->PushGraphicsData(&utilBuffer, sizeof(UtilBuffer), static_cast<int>(CB_TYPE::UTIL));


	FogBuffer fogBuffer;
	fogBuffer.start = 15.f;
	fogBuffer.end = 1000.f;
	NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::FOG))->PushGraphicsData(&fogBuffer, sizeof(FogBuffer), static_cast<int>(CB_TYPE::FOG));

	PushCameraData();
	PushLightData();

	// 스태틱 오브젝트 렌더
	RenderObject();

	// 스킨드 오브젝트 렌더
	RenderSkinned();

	// 그림자 맵 생성
	RenderShadow();
	RenderPointLightShadow();

	SkyBoxPass::Instance.Get().BindIBLTexture();

	// 라이트 렌더
	RenderLight();

	BloomPass::Instance.Get().Bloom();

	// Final 출력
	RenderFinal();
	RenderForward();
	RenderParticle();
	RenderBackBuffer();

	//SkyBoxPass::Instance.Get().Render();

	RenderUI();

	// 디퍼드 정보 출력
	///DrawDeferredInfo();

	// 디퍼드용 SRV UnBind
	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"Deferred_DirectionalLight"))->UnBindGraphicsData();
	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"Deferred_Final"))->UnBindGraphicsData();
	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"BackBufferMaterial"))->UnBindGraphicsData();


}

void RenderSystem::RenderObject()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->OMSetRenderTarget();



	MatrixBuffer matrixBuffer;
	//matrixBuffer.WTM = e.wtm;
	matrixBuffer.VTM = CameraManager::Instance.Get().GetMainCamera()->GetVTM();
	matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM();
	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	matrixBuffer.VTMInv = matrixBuffer.VTM.Invert();
	//matrixBuffer.objectID = DirectX::SimpleMath::Vector4{};
	NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));

	InstancingManager::Instance.Get().RenderStaticDeferred();

	//for (auto& e : this->deferredVec)
	//{
	//	

	//	/*auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(e.mesh->GetName()));

	//	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e.material->GetName()))->PushGraphicsData();
	//	mesh->Render(e.materialIndex);*/
	//	
	//}

	//renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->UnBind();
}

void RenderSystem::RenderSkinned()
{
	MatrixBuffer matrixBuffer;
	//matrixBuffer.WTM = e.wtm;
	matrixBuffer.VTM = CameraManager::Instance.Get().GetMainCamera()->GetVTM();
	matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM();
	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//matrixBuffer.objectID = DirectX::SimpleMath::Vector4{};
	NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));

	InstancingManager::Instance.Get().RenderSkinned();
}

void RenderSystem::RenderShadow()
{
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	ShadowPass::Instance.Get().Bind();

	for (auto& e : lightSet)
	{
		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Directional))
		{
			MatrixBuffer matrixBuffer;
			auto wtm = static_cast<DirectionalLight*>(e)->GetWorldTM();
			wtm._41 = mainCamPos.x;
			wtm._42 = mainCamPos.y;
			wtm._43 = mainCamPos.z;
			auto back = -static_cast<DirectionalLight*>(e)->GetDirection() * 150;
			DirectX::SimpleMath::Vector3 temp;
			temp = back + wtm.Translation();
			wtm._41 = temp.x;
			wtm._42 = temp.y;
			wtm._43 = temp.z;
			matrixBuffer.VTM = wtm.Invert();
			matrixBuffer.PTM = DirectX::XMMatrixOrthographicLH(163.84 * 1.f, 163.84 * 1.f, 1.f, 300);
			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));
		}
	}
	InstancingManager::Instance.Get().RenderStaticShadow();
}

void RenderSystem::RenderPointLightShadow()
{
	auto& lightSet = LightManager::Instance.Get().GetLightList();

	int index = 0;
	
	for (auto& e : lightSet)
	{
		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Point))
		{
			if (e->IsShadowCast() == false)
			{
				PointLightShadowPass::Instance.Get().ClearDSVArray(index);
				continue;
			}

			if (index > MAX_POINT_LIGHT - 1)
			{
				continue;
			}

			// 현재 카메라 프러스텀에 들어온 포인트 라이트만 쉐도우맵을 만들도록 컬링을 진행한다
			auto& frustum = CameraManager::Instance.Get().GetMainCamera()->GetFrustum();

			auto meshName = e->GetMeshName();
			auto mesh = ResourceManager::Instance.Get().GetMesh(e->GetMeshName());

			auto aabb = ResourceManager::Instance.Get().GetMesh(e->GetMeshName())->GetBoundingBox(
				static_cast<PointLight*>(e)->GetWorldTM(), 0);

			if (frustum.Contains(aabb) == DirectX::ContainmentType::DISJOINT)
			{
				continue;
			}

			// Matrix Buffer Set
			DirectX::SimpleMath::Vector3 pos;
			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion quat;
			static_cast<PointLight*>(e)->GetWorldTM().Decompose(scale, quat, pos);

			PointLightVPMatrix pointLightVP;

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
				DirectX::SimpleMath::Matrix viewMatrix = XMMatrixLookAtLH(pos, targetPos, upVec);

				DirectX::SimpleMath::Matrix projMat = CameraManager::Instance.Get().GetPTM90ByResolution(PL_SM_SIZE, PL_SM_SIZE,
					static_cast<PointLight*>(e)->GetLightInfo().farPlane, static_cast<PointLight*>(e)->GetLightInfo().nearPlane);

				pointLightVP.viewProj[i] = viewMatrix * projMat;
			}

			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::POINTLIGHT_VPMATRIX))->PushGraphicsData(&pointLightVP, sizeof(PointLightVPMatrix), static_cast<int>(CB_TYPE::POINTLIGHT_VPMATRIX), true);
			PointLightShadowPass::Instance.Get().Render(index, false);
			InstancingManager::Instance.Get().RenderStaticPointLightShadow(static_cast<PointLight*>(e)->GetWorldTM(), static_cast<PointLight*>(e));
			PointLightShadowPass::Instance.Get().Render(index, true);
			InstancingManager::Instance.Get().RenderSkinnedPointLightShadow(static_cast<PointLight*>(e)->GetWorldTM(), static_cast<PointLight*>(e));
			++index;
		}
	}
	PointLightShadowPass::Instance.Get().EndRender();
}

void RenderSystem::RenderLight()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	// 라이팅 렌더
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->OMSetRenderTarget();

	MatrixBuffer matrixBuffer;
	matrixBuffer.VTM = CameraManager::Instance.Get().GetMainCamera()->GetVTM();
	matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM();
	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;



	int plIndex = 0;
	int lightIndex = 0;
	for (auto& e : lightSet)
	{
		if (e->IsActive() == false)
		{
			lightIndex++;
			continue;
		}

		// Point Light의 경우 실제 Sphere Mesh를 렌더링 파이프라인에 넘긴다.
		// 이때 WVP이 필요하기에 상수버퍼에 밀어넣어야 한다.
		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Directional))
		{
			matrixBuffer.VTMInv = matrixBuffer.VTM.Invert();

			auto wtm = static_cast<DirectionalLight*>(e)->GetWorldTM();
			wtm._41 = mainCamPos.x;
			wtm._42 = mainCamPos.y;
			wtm._43 = mainCamPos.z;
			auto back = -static_cast<DirectionalLight*>(e)->GetDirection() * 150;
			DirectX::SimpleMath::Vector3 temp;
			temp = back + wtm.Translation();
			wtm._41 = temp.x;
			wtm._42 = temp.y;
			wtm._43 = temp.z;

			matrixBuffer.lightVP = wtm.Invert() * DirectX::XMMatrixOrthographicLH(163.84 * 1.f, 163.84 * 1.f, 1.f, 300);
		}
		else if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Point))
		{
			// 만들어진 Shadow Map Bind
			ResourceManager::Instance.Get().GetTexture(L"PointLightShadowDepth")->Bind(23);

			matrixBuffer.VTMInv = matrixBuffer.VTM.Invert();
			matrixBuffer.WTM = static_cast<PointLight*>(e)->GetWorldTM();
			matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
			matrixBuffer.WorldInvTrans = static_cast<PointLight*>(e)->GetWorldTM().Invert().Transpose();

			PointLightIndex plIndexBuffer;

			plIndexBuffer.isShadowCast = e->IsShadowCast();
			if (plIndexBuffer.isShadowCast)
			{
				plIndexBuffer.plIndex = plIndex;
				plIndex++;
			}
			NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::POINTLIGHT_INDEX))->PushGraphicsData(&plIndexBuffer, sizeof(PointLightIndex), static_cast<int>(CB_TYPE::POINTLIGHT_INDEX));
		}

		NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));

		std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e->GetMaterialName()))->SetInt(0, lightIndex);
		std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e->GetMaterialName()))->PushGraphicsData();
		auto mesh = ResourceManager::Instance.Get().GetMesh(e->GetMeshName());
		mesh->Render();

		// 만들어진 Shadow Map UnBind
		ResourceManager::Instance.Get().GetTexture(L"PointLightShadowDepth")->UnBind(23);
		lightIndex++;
	}
}

void RenderSystem::RenderFinal()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::FINAL)]->OMSetRenderTarget();

	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"Deferred_Final"))->PushGraphicsData();
	ResourceManager::Instance.Get().GetMesh(L"Rectangle")->Render();
}

void RenderSystem::RenderBackBuffer()
{
	//ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1,
		ResourceBuilder::Instance.Get().swapChain->GetRTV().GetAddressOf(),
		ResourceBuilder::Instance.Get().swapChain->GetDSV().Get());

	///MatrixBuffer matrixBuffer;
	/////matrixBuffer.WTM = e.wtm;
	///matrixBuffer.VTM = CameraManager::Instance.Get().GetMainCamera()->GetVTM();
	///matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM();
	///matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	///matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	///matrixBuffer.VTMInv = matrixBuffer.VTM.Invert();
	/////matrixBuffer.objectID = DirectX::SimpleMath::Vector4{};
	///NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));

	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"BackBufferMaterial"))->PushGraphicsData();
	ResourceManager::Instance.Get().GetMesh(L"Rectangle")->Render();
}

void RenderSystem::RenderUI()
{
	this->spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, this->commonStates->NonPremultiplied());
	for (auto& i : UIImageSet)
	{
		auto uiImage = std::static_pointer_cast<UIImage>(i);

		if (uiImage->IsActive() == false)
		{
			continue;
		}

		auto texture = ((Texture*)(std::static_pointer_cast<UIImage>(i)->GetTexture()));
		this->spriteBatch->Draw(texture->GetSRV().Get(), uiImage->pos);
	}
	this->spriteBatch->End();


	d2dRT->BeginDraw();
	for (auto& i : this->UITextSet)
	{
		auto uiText = std::static_pointer_cast<UIText>(i);

		if (uiText->IsActive() == false)
		{
			continue;
		}

		D2D1_RECT_F layoutRect = D2D1::RectF(
			uiText->pos.x,
			uiText->pos.y,
			uiText->pos.x + uiText->scale.x,
			uiText->pos.y + uiText->scale.y
		);

		auto brush = QueryBrush(uiText);
		auto textFormat = QueryTextFormat(uiText);

		d2dRT->DrawTextW(
			uiText->text.c_str(), uiText->text.length(), textFormat.Get(), layoutRect, brush.Get()
		);
	}
	d2dRT->EndDraw();
}

void RenderSystem::RenderForward()
{
	//MatrixBuffer matrixBuffer;
	////matrixBuffer.WTM = e.wtm;
	//matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	//matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	//matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	//matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	////matrixBuffer.objectID = DirectX::SimpleMath::Vector4{};
	//NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);

	//for (auto& e : this->forwardVec)
	//{
	//	MatrixBuffer matrixBuffer;
	//	matrixBuffer.WTM = e.wtm;
	//	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	//	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	//	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	//	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//	NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
	//
	//	auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(e.mesh->GetName()));
	//
	//	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e.material->GetName()))->PushGraphicsData();
	//	mesh->Render(e.materialIndex, nullptr);
	//}

	MatrixBuffer matrixBuffer;
	//matrixBuffer.WTM = e.wtm;
	matrixBuffer.VTM = CameraManager::Instance.Get().GetMainCamera()->GetVTM();
	matrixBuffer.PTM = CameraManager::Instance.Get().GetMainCamera()->GetPTM();
	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//matrixBuffer.objectID = DirectX::SimpleMath::Vector4{};
	NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));
	InstancingManager::Instance.Get().RenderStaticForward();
}

void RenderSystem::RenderParticle()
{
	InstancingManager::Instance.Get().RenderParticle();
}

void RenderSystem::DrawDeferredInfo()
{
	auto windowInfo = NailEngine::Instance.Get().GetWindowInfo();
	int width = windowInfo.width;
	int height = windowInfo.height;
	for (int i = 0; i < 9; i++)
	{
		DirectX::SimpleMath::Matrix matSclae = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(width / 10.f, height / 10.f, 1.f));
		DirectX::SimpleMath::Matrix matRotation = DirectX::SimpleMath::Matrix::CreateRotationX(0.f);
		matRotation *= DirectX::SimpleMath::Matrix::CreateRotationY(0.f);
		matRotation *= DirectX::SimpleMath::Matrix::CreateRotationZ(0.f);
		/// 
		DirectX::SimpleMath::Matrix matTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(
			((-width / 2.f) + ((width * 0.05) + ((width / 10.f) * i)))
			, ((height / 2.f) - ((height * 0.05)))
			, 1.f));
		/// 


		if (i == 0)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredPosition"))->PushGraphicsData();
		else if (i == 1)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredNormal"))->PushGraphicsData();
		else if (i == 2)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredColor"))->PushGraphicsData();
		else if (i == 3)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredDepth"))->PushGraphicsData();
		else if (i == 4)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredARM"))->PushGraphicsData();
		else if (i == 5)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredDiffuseLight"))->PushGraphicsData();
		else if (i == 6)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredSpecularLight"))->PushGraphicsData();
		else if (i == 7)
		{
			DirectX::SimpleMath::Matrix matTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(
				((-SM_SIZE / 2.f) + ((SM_SIZE * 0.05) + ((SM_SIZE / 10.f) * i)))
				, ((SM_SIZE / 2.f) - ((SM_SIZE * 0.05)))
				, 1.f)
			);
			matSclae = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(SM_SIZE / 10.f, SM_SIZE / 10.f, 1.f));
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredShadow"))->PushGraphicsData();
		}
		else if (i == 8)
		{
			//matSclae = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(128 / 10.f, 128 / 10.f, 1.f));
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"PBRIrradiance"))->PushGraphicsData();
		}

		DirectX::SimpleMath::Matrix wtm = matSclae * matRotation * matTranslation;
		MatrixBuffer matrixBuffer;
		matrixBuffer.WVP = wtm * DirectX::SimpleMath::Matrix::Identity * CameraManager::Instance.Get().GetMainCamera()->GetVTMOrtho();

		NailEngine::Instance.Get().GetConstantBuffer(static_cast<int>(CB_TYPE::MATRIX))->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), static_cast<int>(CB_TYPE::MATRIX));
		ResourceManager::Instance.Get().GetMesh(L"Rectangle")->Render();
	}
}

void RenderSystem::PushStaticRenderableObject(nail::IRenderable* renderable)
{
	staticMeshRenderInfoMap.insert({ renderable, {} });
	staticMeshRenderInfoMap[renderable].emplace_back(static_cast<StaticMesh*>(renderable)->renderInfoVec[0]);

	deferredSet.insert(static_cast<StaticMesh*>(renderable)->renderInfoVec[0]);
}

void RenderSystem::PopStaticRenderableObject(nail::IRenderable* renderable)
{
	for (int i = 0; i < static_cast<StaticMesh*>(renderable)->renderInfoVec.size(); ++i)
	{
		if (static_cast<StaticMesh*>(renderable)->renderInfoVec[i]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
		{
			InstancingManager::Instance.Get().PopStaticDeferredData(static_cast<StaticMesh*>(renderable)->renderInfoVec[i]);
		}
		else
		{
			InstancingManager::Instance.Get().PopStaticForwardData(static_cast<StaticMesh*>(renderable)->renderInfoVec[i]);
		}
	}

	this->staticMeshRenderInfoMap.erase(renderable);
}

void RenderSystem::PushSkinnedRenderableObject(nail::IRenderable* renderable)
{
	skinnedMeshRenderInfoMap.insert({ renderable, {} });
	skinnedMeshRenderInfoMap[renderable].emplace_back(static_cast<SkinnedMesh*>(renderable)->renderInfoVec[0]);

	skinnedSet.insert(static_cast<SkinnedMesh*>(renderable)->renderInfoVec[0]);
}

void RenderSystem::PopSkinnedRenderableObject(nail::IRenderable* renderable)
{
	for (int i = 0; i < static_cast<SkinnedMesh*>(renderable)->renderInfoVec.size(); ++i)
	{
		InstancingManager::Instance.Get().PopSkinnedData(static_cast<SkinnedMesh*>(renderable)->renderInfoVec[i]);
		this->skinnedSet.erase(static_cast<SkinnedMesh*>(renderable)->renderInfoVec[i]);
	}

	this->skinnedMeshRenderInfoMap.erase(renderable);
}

void RenderSystem::PushUIObject(std::shared_ptr<nail::IRenderable> renderable)
{
	this->UIImageSet.insert(renderable);
}

void RenderSystem::PopUIObject(std::shared_ptr<nail::IRenderable> renderable)
{
	this->UIImageSet.erase(renderable);
}

void RenderSystem::PushTextObject(std::shared_ptr<nail::IRenderable> renderable)
{
	this->UITextSet.insert(renderable);
}

void RenderSystem::PopTextObject(std::shared_ptr<nail::IRenderable> renderable)
{
	this->UITextSet.erase(renderable);
}

void RenderSystem::ReSortUIObject(int layer, std::shared_ptr<nail::IRenderable> ui)
{
	auto iter = this->UIImageSet.find(ui);

	assert(iter != this->UIImageSet.end());

	if (iter != this->UIImageSet.end())
	{
		std::static_pointer_cast<UIImage>(*iter)->layer = layer;
		auto newUI = *iter;
		this->UIImageSet.erase(iter);
		this->UIImageSet.insert(newUI);
	}
}

void RenderSystem::ReSortRenderInfo(nail::IRenderable* renderable, int index)
{
	if (staticMeshRenderInfoMap[renderable][index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
	{
		auto iter = forwardSet.find(staticMeshRenderInfoMap[renderable][index]);
		if (iter == forwardSet.end())
		{
			return;
		}
		else
		{
			// 포워드에서 디퍼드로
			InstancingManager::Instance.Get().PopStaticForwardData(staticMeshRenderInfoMap[renderable][index]);
			forwardSet.erase(iter);
			deferredSet.insert(staticMeshRenderInfoMap[renderable][index]);
			InstancingManager::Instance.Get().RegisterStaticDeferredData(staticMeshRenderInfoMap[renderable][index]);
		}
	}
	else
	{
		auto iter = deferredSet.find(staticMeshRenderInfoMap[renderable][index]);
		if (iter == deferredSet.end())
		{
			return;
		}
		else
		{
			// 디퍼드에서 포워드로
			InstancingManager::Instance.Get().PopStaticDeferredData(staticMeshRenderInfoMap[renderable][index]);
			deferredSet.erase(iter);
			forwardSet.insert(staticMeshRenderInfoMap[renderable][index]);
			InstancingManager::Instance.Get().RegisterStaticForwardData(staticMeshRenderInfoMap[renderable][index]);
		}
	}
}

void RenderSystem::RegisterRenderInfo(nail::IRenderable* renderable, std::shared_ptr<RenderInfo> renderInfo)
{
	auto iter = staticMeshRenderInfoMap.find(renderable);
	if (iter != staticMeshRenderInfoMap.end())
	{
		if (staticMeshRenderInfoMap[renderable].size() <= renderInfo->materialIndex)
		{
			staticMeshRenderInfoMap[renderable].emplace_back(renderInfo);
		}

		if (renderInfo->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
		{
			deferredSet.insert(renderInfo);

			InstancingManager::Instance.Get().RegisterStaticDeferredData(renderInfo);
		}
		else if (renderInfo->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
		{
			forwardSet.insert(renderInfo);

			InstancingManager::Instance.Get().RegisterStaticForwardData(renderInfo);
		}
	}
}

void RenderSystem::RegisterSkinnedRenderInfo(nail::IRenderable* renderable, std::shared_ptr<SkinnedRenderInfo> renderInfo)
{
	auto iter = skinnedMeshRenderInfoMap.find(renderable);
	if (iter != skinnedMeshRenderInfoMap.end())
	{
		skinnedMeshRenderInfoMap[renderable].emplace_back(renderInfo);
		skinnedSet.insert(renderInfo);

		InstancingManager::Instance.Get().RegisterSkinnedData(renderInfo);
	}
}

Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> RenderSystem::QueryBrush(std::shared_ptr<UIText> uiText)
{
	auto iter = this->brushMap.find(uiText->color);
	if (iter != this->brushMap.end())
	{
		return iter->second;
	}

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> tempBrush;
	D2D1::ColorF colorf = D2D1::ColorF(uiText->color.r, uiText->color.g, uiText->color.b, uiText->color.a);

	this->d2dRT->CreateSolidColorBrush(
		colorf
		, tempBrush.GetAddressOf());

	this->brushMap.insert({ uiText->color, tempBrush });

	return this->brushMap[uiText->color];
}

Microsoft::WRL::ComPtr<IDWriteTextFormat> RenderSystem::QueryTextFormat(std::shared_ptr<UIText> uiText)
{
	auto iter = this->wFormatMap.find(uiText->key);
	if (iter != this->wFormatMap.end())
	{
		return iter->second;
	}

	Microsoft::WRL::ComPtr<IDWriteTextFormat> tempFormat;

	wFactory->CreateTextFormat(
		uiText->key.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		uiText->size,
		uiText->font.c_str(),
		tempFormat.GetAddressOf()
	);

	tempFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	tempFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	this->wFormatMap.insert({ uiText->key, tempFormat });

	return this->wFormatMap[uiText->key];
}

