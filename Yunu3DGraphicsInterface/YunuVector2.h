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

		static Vector2 Lerp(const Vector2& start, const Vector2& end, float t)
		{
			return Vector2
			(
				start.x + t * (end.x - start.x),
				start.y + t * (end.y - start.y)
			);
		}
	};
}
