#pragma once

namespace yunuGI
{
	struct Vector4
	{
		float x;
		float y;
		float z;
		float w;

		Vector4 operator*(const yunuGI::Matrix4x4& mat)
		{
			Vector4 result;
			result.x = x * mat.m11 + y * mat.m21 + z * mat.m31 + w * mat.m41;
			result.y = x * mat.m12 + y * mat.m22 + z * mat.m32 + w * mat.m42;
			result.z = x * mat.m13 + y * mat.m23 + z * mat.m33 + w * mat.m43;
			result.w = x * mat.m14 + y * mat.m24 + z * mat.m34 + w * mat.m44;
			return result;
		}
	};

	
}
