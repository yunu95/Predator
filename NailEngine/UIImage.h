#pragma once
#include "IRenderable.h"

class UIImage : public IRenderable
{
public:
	void SetTexture(yunuGI::ITexture* texture)
	{
		this->texture = texture;
	}

	yunuGI::ITexture* GetTexture()
	{
		return this->texture;
	}

	bool operator<(const UIImage& other) const
	{
		return layer < other.layer;
	}

public:
	DirectX::SimpleMath::Vector2 pos;
	int layer = 0;

private:
	yunuGI::ITexture* texture;
};

struct CompareSmartPtr {
	bool operator()(const std::shared_ptr<IRenderable>& lhs, const std::shared_ptr<IRenderable>& rhs) const {
		const auto& leftImage = std::static_pointer_cast<UIImage>(lhs);
		const auto& rightImage = std::static_pointer_cast<UIImage>(rhs);

		if (leftImage->layer != rightImage->layer) {
			return leftImage->layer < rightImage->layer; // 다른 layer인 경우에만 비교
		}

		// 나중에 이미지의 layer가 이상하다면 이 코드를 볼 것 주소로 비교하기에 어떤 동작이 일어날 지 확신할 수 없음

		return leftImage.get() < rightImage.get();
	}
};