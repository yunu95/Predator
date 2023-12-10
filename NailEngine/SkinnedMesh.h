#pragma once

#include "IRenderable.h"

class SkinnedMesh : public IRenderable
{
public:
	void SetBone(std::wstring fbxName)
	{
		this->fbxName = fbxName;
	}

	const std::wstring& GetBone()
	{
		return this->fbxName;
	}

private:
	std::wstring fbxName;
};

