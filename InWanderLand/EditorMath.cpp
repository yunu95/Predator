#include "EditorMath.h"

#include <DirectXMath.h>

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

			yunuGI::Matrix4x4 ConvertWTM(const glm::mat4& mat)
			{
				glm::vec3 scale;
				glm::quat rotation;
				glm::vec3 position;
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(mat, scale, rotation, position, skew, perspective);

				auto frot = ConvertQuaternion(rotation);
				auto fpos = ConvertVector3(position);

				DirectX::XMFLOAT4 ffrot = DirectX::XMFLOAT4(frot.x, frot.y, frot.z, frot.w);

				auto finalRot = DirectX::XMLoadFloat4(&ffrot);
				auto finalPos = DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&fpos));

				auto stm = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
				auto rtm = DirectX::XMMatrixRotationQuaternion(finalRot);
				auto ttm = DirectX::XMMatrixTranslationFromVector(finalPos);

				auto ftm = stm * rtm * ttm;

				return *reinterpret_cast<yunuGI::Matrix4x4*>(&ftm);
			}

			glm::mat4 ConvertWTM(const yunuGI::Matrix4x4& mat)
			{
				auto ftm = *reinterpret_cast<DirectX::XMMATRIX*>(const_cast<yunuGI::Matrix4x4*>(&mat));
				DirectX::XMVECTOR scale;
				DirectX::XMVECTOR rotation;
				DirectX::XMVECTOR position;

				DirectX::XMMatrixDecompose(&scale, &rotation, &position, ftm);

				yunuGI::Vector4 fs = *reinterpret_cast<yunuGI::Vector4*>(&scale);
				yunuGI::Vector4 fr = *reinterpret_cast<yunuGI::Vector4*>(&rotation);
				yunuGI::Vector4 fp = *reinterpret_cast<yunuGI::Vector4*>(&position);
				
				fr.x *= -1;
				fr.y *= -1;
				auto finalMat = glm::translate(glm::mat4(1.0f), glm::vec3(fp.x, fp.y, -fp.z)) * glm::mat4_cast(*reinterpret_cast<glm::quat*>(&fr)) * glm::scale(glm::mat4(1.0f), glm::vec3(fs.x, fs.y, fs.z));

				return finalMat;
			}

			yunuGI::Matrix4x4 ConvertVTM(const glm::mat4& mat)
			{
				auto gwtm = glm::inverse(mat);
				return GetInverseMatrix(ConvertWTM(gwtm));
			}

			glm::mat4 ConvertVTM(const yunuGI::Matrix4x4& mat)
			{
				auto wtm = GetInverseMatrix(mat);
				return glm::inverse(ConvertWTM(wtm));
			}

			yunuGI::Matrix4x4 ConvertPTM(const glm::mat4& mat)
			{
				yunuGI::Matrix4x4 finalTM;
				finalTM.m11 = mat[0][0]; finalTM.m12 = mat[0][1]; finalTM.m13 = mat[0][2]; finalTM.m14 = mat[0][3];
				finalTM.m21 = mat[1][0]; finalTM.m22 = mat[1][1]; finalTM.m23 = mat[1][2]; finalTM.m24 = mat[1][3];
				finalTM.m31 = mat[2][0]; finalTM.m32 = mat[2][1]; finalTM.m33 = -mat[2][2]; finalTM.m34 = -mat[2][3];
				finalTM.m41 = mat[3][0]; finalTM.m42 = mat[3][1]; finalTM.m43 = mat[3][2]; finalTM.m44 = mat[3][3];
				return finalTM;
			}

			glm::mat4 ConvertPTM(const yunuGI::Matrix4x4& mat)
			{
				glm::mat4 finalTM;
				finalTM[0][0] = mat.m11; finalTM[0][1] = mat.m12; finalTM[0][2] = mat.m13; finalTM[0][3] = mat.m14;
				finalTM[1][0] = mat.m21; finalTM[1][1] = mat.m22; finalTM[1][2] = mat.m23; finalTM[1][3] = mat.m24;
				finalTM[2][0] = mat.m31; finalTM[2][1] = mat.m32; finalTM[2][2] = -mat.m33; finalTM[2][3] = -mat.m34;
				finalTM[3][0] = mat.m41; finalTM[3][1] = mat.m42; finalTM[3][2] = mat.m43; finalTM[3][3] = mat.m44;
				return finalTM;
			}

			yunuGI::Quaternion ConvertQuaternion(const glm::quat& quat)
			{
				yunuGI::Quaternion finalQuat;
				finalQuat.x = -quat.x;
				finalQuat.y = -quat.y;
				finalQuat.z = quat.z;
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
				finalMat.m12 = 2.0f * quat.x * quat.y + 2.0f * + quat.z * quat.w;
				finalMat.m13 = 2.0f * -quat.x * quat.z + 2.0f * quat.y * quat.w;
				finalMat.m14 = 0.0f;

				finalMat.m21 = 2.0f * quat.x * quat.y - 2.0f * quat.z * quat.w;
				finalMat.m22 = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.z * quat.z;
				finalMat.m23 = 2.0f * -quat.y * quat.z - 2.0f * quat.x * quat.w;
				finalMat.m24 = 0.0f;

				finalMat.m31 = 2.0f * -quat.x * quat.z - 2.0f * quat.y * quat.w;
				finalMat.m32 = 2.0f * -quat.y * quat.z + 2.0f * quat.x * quat.w;
				finalMat.m33 = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.y * quat.y;;
				finalMat.m34 = 0.0f;

				finalMat.m41 = 0.0f;
				finalMat.m42 = 0.0f;
				finalMat.m43 = 0.0f;
				finalMat.m44 = 1.0f;
				return finalMat;
			}

			yunuGI::Matrix4x4 GetInverseMatrix(const yunuGI::Matrix4x4& mat)
			{
				DirectX::XMMATRIX wtm = *reinterpret_cast<DirectX::XMMATRIX*>(const_cast<yunuGI::Matrix4x4*>(&mat));
				DirectX::XMVECTOR d;
				auto im = DirectX::XMMatrixInverse(&d, wtm);
				return *reinterpret_cast<yunuGI::Matrix4x4*>(&im);
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
