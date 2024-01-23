#include "EditorMath.h"

namespace application
{
	namespace editor
	{
		namespace math
		{
			yunuGI::Vector3 ConvertVector3(const glm::vec3& vec)
			{
				yunuGI::Vector3 finalVec;
				finalVec.x = vec.x;
				finalVec.y = vec.y;
				finalVec.z = -vec.z;
				return finalVec;
			}

			yunuGI::Vector4 ConvertVector4(const glm::vec4& vec)
			{
				yunuGI::Vector4 finalVec;
				finalVec.w = vec.w;
				finalVec.x = vec.x;
				finalVec.y = vec.y;
				finalVec.z = -vec.z;
				return finalVec;
			}

			yunuGI::Matrix4x4 ConvertMatrix4x4(const glm::mat4& mat)
			{
				yunuGI::Matrix4x4 finalMat;
				finalMat.m11 = mat[0][0]; finalMat.m12 = mat[1][0]; finalMat.m13 = mat[2][0]; finalMat.m14 = mat[3][0];
				finalMat.m21 = mat[0][1]; finalMat.m22 = mat[1][1]; finalMat.m23 = mat[2][1]; finalMat.m24 = mat[3][1];
				finalMat.m31 = -mat[0][2]; finalMat.m32 = -mat[1][2]; finalMat.m33 = -mat[2][2]; finalMat.m34 = -mat[3][2];
				finalMat.m41 = mat[0][3]; finalMat.m42 = mat[1][3]; finalMat.m43 = mat[2][3]; finalMat.m44 = mat[3][3];
				return finalMat;
			}

			yunuGI::Quaternion ConvertQuaternion(const glm::quat& quat)
			{
				yunuGI::Quaternion finalQuat;
				finalQuat.x = quat.x;
				finalQuat.y = quat.y;
				finalQuat.z = -quat.z;
				finalQuat.w = quat.w;
				return finalQuat;
			}

			yunuGI::Matrix4x4 GetRotationMatrix(const yunuGI::Quaternion& quat)
			{
				yunuGI::Matrix4x4 finalMat;
				finalMat.m11 = 1.0f - 2.0f * quat.y * quat.y - 2.0f * quat.z * quat.z;
				finalMat.m12 = 2.0f * quat.x * quat.y + 2.0f * quat.z * quat.w;
				finalMat.m13 = 2.0f * quat.x * quat.z - 2.0f * quat.y * quat.w;
				finalMat.m14 = 0.0f;

				finalMat.m21 = 2.0f * quat.x * quat.y - 2.0f * quat.z * quat.w;
				finalMat.m22 = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.z * quat.z;
				finalMat.m23 = 2.0f * quat.y * quat.z + 2.0f * quat.x * quat.w;
				finalMat.m24 = 0.0f;

				finalMat.m31 = 2.0f * quat.x * quat.z + 2.0f * quat.y * quat.w;
				finalMat.m32 = 2.0f * quat.y * quat.z - 2.0f * quat.x * quat.w;
				finalMat.m33 = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.y * quat.y;;
				finalMat.m34 = 0.0f;

				finalMat.m41 = 0.0f;
				finalMat.m42 = 0.0f;
				finalMat.m43 = 0.0f;
				finalMat.m44 = 1.0f;

				return finalMat;
			}

			yunuGI::Matrix4x4 GetRotationMatrix(const glm::quat& quat)
			{
				yunuGI::Matrix4x4 finalMat;
				finalMat.m11 = 1.0f - 2.0f * quat.y * quat.y - 2.0f * quat.z * quat.z;
				finalMat.m12 = 2.0f * quat.x * quat.y + 2.0f * -quat.z * quat.w;
				finalMat.m13 = 2.0f * quat.x * -quat.z - 2.0f * quat.y * quat.w;
				finalMat.m14 = 0.0f;

				finalMat.m21 = 2.0f * quat.x * quat.y - 2.0f * -quat.z * quat.w;
				finalMat.m22 = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.z * quat.z;
				finalMat.m23 = 2.0f * quat.y * -quat.z + 2.0f * quat.x * quat.w;
				finalMat.m24 = 0.0f;

				finalMat.m31 = 2.0f * quat.x * -quat.z + 2.0f * quat.y * quat.w;
				finalMat.m32 = 2.0f * quat.y * -quat.z - 2.0f * quat.x * quat.w;
				finalMat.m33 = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.y * quat.y;;
				finalMat.m34 = 0.0f;

				finalMat.m41 = 0.0f;
				finalMat.m42 = 0.0f;
				finalMat.m43 = 0.0f;
				finalMat.m44 = 1.0f;
				return finalMat;
			}

			yunuGI::Vector3 RotateVector3(const yunuGI::Vector3& vec, const yunuGI::Matrix4x4& roMat)
			{
				yunuGI::Vector3 finalVec;

				finalVec.x = roMat.m11 * vec.x + roMat.m21 * vec.y + roMat.m31 * vec.z + roMat.m41 * 1.0f;
				finalVec.y = roMat.m12 * vec.x + roMat.m22 * vec.y + roMat.m32 * vec.z + roMat.m42 * 1.0f;
				finalVec.z = roMat.m13 * vec.x + roMat.m23 * vec.y + roMat.m33 * vec.z + roMat.m43 * 1.0f;

				return finalVec;
			}
		}
	}
}
