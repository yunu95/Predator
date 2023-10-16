#pragma once
#include "Component.h"
#include "Vector3.h"

class MouseDragRect : public yunutyEngine::Component
{
public:
	static MouseDragRect* PlaceRect(yunutyEngine::Vector3d position, float width, float height);
	float m_width;
	float m_height;
};

