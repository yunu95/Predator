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

		Vector3 operator + (const Vector3& rhs)
		{
			Vector3 tempVec;
			tempVec.x =  this->x + rhs.x;
			tempVec.y =  this->y + rhs.y;
			tempVec.z =  this->z + rhs.z;
			return tempVec;
		}

		Vector3 operator* (const float& val) const // 수정: const 멤버 함수로 변경
		{
			return { this->x * val, this->y * val, this->z * val }; // 수정: 벡터의 각 요소에 스칼라 값 곱한 후 반환
		}

		static float Length(const Vector3& vector) // 수정: 정적 멤버 함수로 변경
		{
			return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
		}

		static Vector3 Normalize(const Vector3& vector) // 수정: 정적 멤버 함수로 변경
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
