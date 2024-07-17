#include "InWanderLand.h"
#include "BurnEffect.h"


void BurnEffect::SetDuration(float duration)
{
	this->duration = duration;
}

void BurnEffect::Init()
{
	// Init은 데이터 셋팅용 함수
	// 여기서 Apear를 켜준다.
	//this->Appear();

	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	auto rootObj = GetGameObject();

	for (auto& each : rootObj->GetChildren())
	{
		yunutyEngine::graphics::SkinnedMesh* renderer = nullptr;
		renderer = each->GetComponent<yunutyEngine::graphics::SkinnedMesh>();

		if (renderer)
		{
			this->renderer = renderer;

			for (int i = 0; i < renderer->GetGI().GetMaterialCount(); ++i)
			{
				auto material = renderer->GetGI().GetMaterial(i, false);

				this->originMaterialVec.push_back(material);

				std::wstring burnMaterialName = L"Burn_";
				burnMaterialName += material->GetName();

				auto burnMaterial = _resourceManager->CloneMaterial(burnMaterialName, material);
				burnMaterial->SetTexture(yunuGI::Texture_Type::Temp0, _resourceManager->GetTexture(L"Texture/Dissolve.jpg"));
				burnMaterial->SetPixelShader(_resourceManager->GetShader(L"DissolvePS.cso"));
				burnMaterial->SetVertexShader(_resourceManager->GetShader(L"DissolveVS.cso"));

				this->burnMaterialVec.push_back(burnMaterial);
			}
		}
	}

	// 이 부분이 실제로 적용되는 부분 처음에는 나타날 것이기 때문에 amount를 1로 설정
	if (isFirst)
	{
		isFirst = false;

		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().SetMaterial(i, burnMaterialVec[i]);
		}
	}

	for (int i = 0; i < originMaterialVec.size(); ++i)
	{
		renderer->GetGI().GetMaterial(i)->SetFloat(0, defaultStart * GetGameObject()->GetTransform()->GetWorldScale().x);
		renderer->GetGI().GetMaterial(i)->SetFloat(1, 0);
	}
}

void BurnEffect::Disappear()
{
	isDisAppear = true;
	isAppear = false;
	isDone = false;
	isFirst = true;
	amount = 0.f;
	uv = 0.f;
	accTime = 0.f;
	if (isFirst)
	{
		isFirst = false;

		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().SetMaterial(i, burnMaterialVec[i]);
		}
	}

	for (int i = 0; i < originMaterialVec.size(); ++i)
	{
		renderer->GetGI().GetMaterial(i)->SetFloat(0, defaultEnd * GetGameObject()->GetTransform()->GetWorldScale().x);
		renderer->GetGI().GetMaterial(i)->SetFloat(1, 0);
	}
}

void BurnEffect::Appear()
{
	isDisAppear = false;
	isAppear = true;
	isDone = false;
	isFirst = true;

	this->accTime = 0.f;
	this->amount = 0.f;
	this->uv = 0.f;
}

bool BurnEffect::IsDone()
{
	return this->isDone;
}

void BurnEffect::OnEnable()
{
	//if (isFirst)
	//{
	//	this->amount = 1.f;
	//	isFirst = false;

	//	for (int i = 0; i < originMaterialVec.size(); ++i)
	//	{
	//		renderer->GetGI().SetMaterial(i, burnMaterialVec[i]);
	//		renderer->GetGI().GetMaterial(i)->SetColor(edgeColor);
	//		renderer->GetGI().GetMaterial(i)->SetFloat(1, this->edgeThickness);
	//	}
	//}

	//for (int i = 0; i < originMaterialVec.size(); ++i)
	//{
	//	renderer->GetGI().GetMaterial(i)->SetFloat(0, amount);
	//}
}

void BurnEffect::OnDisable()
{
	if (isFirst)
	{
		isFirst = false;

		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().SetMaterial(i, burnMaterialVec[i]);
		}
	}

	for (int i = 0; i < originMaterialVec.size(); ++i)
	{
		renderer->GetGI().GetMaterial(i)->SetFloat(0, this->defaultStart * GetGameObject()->GetTransform()->GetWorldScale().x);
		renderer->GetGI().GetMaterial(i)->SetFloat(1, 0);
	}
}

void BurnEffect::Start()
{

}

void BurnEffect::Update()
{
	if (this->isPause) return;

	if (isAppear && !isDone)
	{
		if (duration == 0)
		{
			for (int i = 0; i < originMaterialVec.size(); ++i)
			{
				renderer->GetGI().SetMaterial(i, originMaterialVec[i], true);
			}
			isDone = true;
			return;
		}
		
		// 1 ~ threshold : 나타남
		float scale = GetGameObject()->GetTransform()->GetWorldScale().x;

		accTime += Time::GetDeltaTime();
		uv += 0.2 * Time::GetDeltaTime();
		float t = accTime / duration;
		
		amount = yunutyEngine::math::LerpF(this->defaultStart * scale, this->defaultEnd * scale,t);

		
		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().GetMaterial(i)->SetFloat(0, amount);
			renderer->GetGI().GetMaterial(i)->SetFloat(1, uv);
		}


		if (t >= 1.f)
		{
			amount = 0;
			uv = 0;
			isDone = true;
			// 나타나는 연출이 다 끝나면 원래 머터리얼로 돌려줌
			for (int i = 0; i < originMaterialVec.size(); ++i)
			{
				renderer->GetGI().SetMaterial(i, originMaterialVec[i], true);
			}
		}
	}
	else if (isDisAppear && !isDone)
	{
		if (duration == 0)
		{
			for (int i = 0; i < originMaterialVec.size(); ++i)
			{
				renderer->GetGI().SetMaterial(i, originMaterialVec[i], true);
			}
			isDone = true;
			return;
		}

		// threshold ~ 1 : 사라짐
		float scale = GetGameObject()->GetTransform()->GetWorldScale().x;

		accTime += Time::GetDeltaTime();
		uv += 0.2 * Time::GetDeltaTime();
		float t = accTime / duration;

		amount = yunutyEngine::math::LerpF(this->defaultEnd * scale, this->defaultStart * scale, t);

		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().GetMaterial(i)->SetFloat(0, amount);
			renderer->GetGI().GetMaterial(i)->SetFloat(1, uv);
		}

		if (amount >= 1)
		{
			amount = 0;
			uv = 0;
			isDone = true;
		}
	}
}

void BurnEffect::Reset()
{
	//isDisAppear = false;
	//isAppear = false;
	//isFirst = true;
	//isDone = true;
	//edgeThickness = 0.01f;
	////duration = 0.2f;
	//amount = 0.f;
	//
	//for (int i = 0; i < originMaterialVec.size(); ++i)
	//{
	//	renderer->GetGI().SetMaterial(i, originMaterialVec[i], true);
	//}
}

void BurnEffect::Pause()
{
	this->isPause = true;
}

void BurnEffect::Resume()
{
	this->isPause = false;
}
