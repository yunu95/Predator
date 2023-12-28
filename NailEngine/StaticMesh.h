#pragma once
#include "IRenderable.h"

class StaticMesh : public IRenderable
{
public:
	~StaticMesh()
	{
		int a = 1;
	}
};

