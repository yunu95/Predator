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

		float Length(const Vector3& vector)
		{
			return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
		}

		Vector3 Normalize(const Vector3& vector)
		{
			float length = Length(vector);
			if (length != 0)
			{
				float invLength = 1.0f / length;
				return { vector.x * invLength, vector.y * invLength, vector.z * invLength };
			}
			else
			{
				return { 0.0f, 0.0f, 0.0f }; 
			}
		}
	};
}
