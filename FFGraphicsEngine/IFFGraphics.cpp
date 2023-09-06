#include "GraphicsPch.h"
#include "IFFGraphics.h"
#include "FFGraphicsEngine.h"

IFFGraphics* CreateFFRenderer()
{
	return nullptr;
}

void DeleteFFRenderer(IFFGraphics** p_renderer)
{
	delete *p_renderer;
}
