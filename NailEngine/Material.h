#pragma once

#include "YunuGraphicsInterface.h"

#include <string>
#include <array>

#include "Struct.h"

class Material
{
public:
	Material();
	~Material();

public:
#pragma region Setter
	void SetMaterialName(const yunuGI::MaterialDesc& materialDesc) { this->materialDesc = materialDesc; }

#pragma endregion

	void PushGraphicsData();

private:
	yunuGI::MaterialDesc materialDesc;

	std::wstring vs;
	std::wstring ps;
	std::array<std::wstring, MAX_TEXTURE> textures;

	MaterialBuffer materialBuffer;
};

