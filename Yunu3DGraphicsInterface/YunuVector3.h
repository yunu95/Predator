#pragma once

namespace yunuGI
{
	struct Vector3
	{
		float x;
		float y;
		float z;

		void operator += (const Vector3& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
		}

		Vector3 operator* (const double& val)
		{
			this->x *= val;
			this->y *= val;
			this->z *= val;

			return *this;
		}
	};
}
