#pragma once

namespace yunuGI
{
	struct Vector2
	{
		float x;
		float y;
		Vector2(float x, float y)
		{
			this->x = x;
			this->y = y;
		}

		Vector2()
		{
			this->x = 0;
			this->y = 0;
		}

		Vector2 operator + (float val)
		{
			Vector2 temp = *this;
			temp.x += val;
			temp.y += val;
			return temp;
		}
	};
}
