#pragma once
#include "IRenderable.h"

class UIText : public IRenderable
{
public:
	UIText()
	{
		size = 18;
		font = L"ko-kr";
		color = yunuGI::Color{ 0,0,0,1 };
		key = font + L"_"  + std::to_wstring(size);
	}

	void SetFontSize(int size)
	{
		this->size = size;
		key = font + L"_" + std::to_wstring(size);
	};

	void SetFont(std::wstring font)
	{
		this->font = font;
		key = font + L"_" + std::to_wstring(size);
	};

	void SetColor(yunuGI::Color color)
	{
		this->color = color;
	};

	std::wstring& GetKey()
	{
		return this->key;
	}

public:
	DirectX::SimpleMath::Vector2 pos;
	DirectX::SimpleMath::Vector2 scale;

	std::wstring text;

	int size;
	std::wstring font;
	yunuGI::Color color;
	std::wstring key;
};