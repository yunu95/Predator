#pragma once

#include <string>
#include <array>

#include "Struct.h"

#define MAX_TEXTURE 7

class Material
{
public:
	Material();
	~Material();

public:
#pragma region Setter
	void SetMaterialName(const std::wstring& materialName) { this->materialName = materialName; }

#pragma endregion

	void PushGraphicsData();

private:
	std::wstring materialName;

	MaterialBuffer materialBuffer;

	std::wstring vs;
	std::wstring ps;
	std::array<std::wstring, MAX_TEXTURE> textures;

};

