#include "InWanderLand.h"
#include "BurnEffect.h"

void BurnEffect::SetEdgeColor(yunuGI::Color color)
{
	this->edgeColor = color;
}

void BurnEffect::SetEdgeThickness(float edgeThickness)
{
	this->edgeThickness = edgeThickness;
}

void BurnEffect::SetDuration(float duration)
{
	this->duration = duration;
}

void BurnEffect::Init()
{
	// Init은 데이터 셋팅용 함수
	// 여기서 Apear를 켜준다.
	this->Appear();

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

				this->burnMaterialVec.push_back(burnMaterial);
			}
		}
	}

	// 이 부분이 실제로 적용되는 부분 처음에는 나타날 것이기 때문에 amount를 1로 설정
	if (isFirst)
	{
		this->amount = 1.f;
		isFirst = false;

		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().SetMaterial(i, burnMaterialVec[i]);
			renderer->GetGI().GetMaterial(i)->SetColor(edgeColor);
			renderer->GetGI().GetMaterial(i)->SetFloat(1, this->edgeThickness);
		}
	}

	for (int i = 0; i < originMaterialVec.size(); ++i)
	{
		renderer->GetGI().GetMaterial(i)->SetFloat(0, amount);
	}
}

void BurnEffect::Disappear()
{
	isDisAppear = true;
	isAppear = false;
	isDone = false;
	isFirst = true;

	if (isFirst)
	{
		this->amount = 0;
		isFirst = false;

		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().SetMaterial(i, burnMaterialVec[i]);
			renderer->GetGI().GetMaterial(i)->SetColor(edgeColor);
			renderer->GetGI().GetMaterial(i)->SetFloat(1, this->edgeThickness);
		}
	}

	for (int i = 0; i < originMaterialVec.size(); ++i)
	{
		renderer->GetGI().GetMaterial(i)->SetFloat(0, amount);
	}
}

void BurnEffect::Appear()
{
	isDisAppear = false;
	isAppear = true;
	isDone = false;
	isFirst = true;
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
	// 오브젝트가 비활성화 될 땐 다음에 다시 켜지는것을 기대하고 amount를 1로 돌리고 셋팅한다.

	if (isFirst)
	{
		this->amount = 1.f;
		isFirst = false;

		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().SetMaterial(i, burnMaterialVec[i]);
			renderer->GetGI().GetMaterial(i)->SetColor(edgeColor);
			renderer->GetGI().GetMaterial(i)->SetFloat(1, this->edgeThickness);
		}
	}

	for (int i = 0; i < originMaterialVec.size(); ++i)
	{
		renderer->GetGI().GetMaterial(i)->SetFloat(0, amount);
	}
}

void BurnEffect::Start()
{

}

void BurnEffect::Update()
{
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

		// 1 ~ 0 -> 나타남
		amount -= (1 / duration * Time::GetDeltaTime());

		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().GetMaterial(i)->SetFloat(0, amount);
		}


		if (amount <= 0)
		{
			amount = 0;
			isDone = true;
			for (int i = 0; i < originMaterialVec.size(); ++i)
			{
				renderer->GetGI().SetMaterial(i, originMaterialVec[i], true);
			}
		}
	}
	else if(isDisAppear && !isDone)
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

		// 0 ~ 1 -> 사라짐
		amount += (1 / duration * Time::GetDeltaTime());

		if (amount >= 1)
		{
			//amount = 1;
			isDone = true;
			//for (int i = 0; i < originMaterialVec.size(); ++i)
			//{
			//	renderer->GetGI().SetMaterial(i, originMaterialVec[i], true);
			//}
		}

		for (int i = 0; i < originMaterialVec.size(); ++i)
		{
			renderer->GetGI().GetMaterial(i)->SetFloat(0, amount);
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
