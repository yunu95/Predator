/// 2024. 01. 22 김상준
/// OpenGL 기반의 GLM 라이브러리를 지원하는 수학 계산 모듈
/// yunuGI 값으로의 변환을 지원함

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/constants.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "YunuGraphicsInterface.h"

namespace application
{
	namespace editor
	{
		namespace math
		{
			double GetPI();

			int GetIntMax();
			int GetIntMin();
			float GetFloatMax();
			float GetFloatMin();
			double GetDoubleMax();
			double GetDoubleMin();

			yunuGI::Vector3 ConvertVector3(const glm::vec3& vec);
			yunuGI::Vector4 ConvertVector4(const glm::vec4& vec);
			yunuGI::Matrix4x4 ConvertWTM(const glm::mat4& mat);
			glm::mat4 ConvertWTM(const yunuGI::Matrix4x4& mat);
			yunuGI::Matrix4x4 ConvertVTM(const glm::mat4& mat);
			glm::mat4 ConvertVTM(const yunuGI::Matrix4x4& mat);
			yunuGI::Matrix4x4 ConvertPTM(const glm::mat4& mat);
			glm::mat4 ConvertPTM(const yunuGI::Matrix4x4& mat);
			yunuGI::Quaternion ConvertQuaternion(const glm::quat& quat);

			yunuGI::Matrix4x4 GetTranslateMatrix(const yunuGI::Vector3& trans);
			yunuGI::Matrix4x4 GetRotationMatrix(const yunuGI::Quaternion& quat);
			yunuGI::Matrix4x4 GetScaleMatrix(const yunuGI::Vector3& scale);
			yunuGI::Matrix4x4 GetInverseMatrix(const yunuGI::Matrix4x4& mat);

			yunuGI::Vector3 GetEulerAngle(const yunuGI::Quaternion& quat);
			yunuGI::Quaternion GetQuaternion(const yunuGI::Vector3& euler);

			yunuGI::Vector3 RotateVector3(const yunuGI::Vector3& vec, const yunuGI::Matrix4x4& roMat);

			void DecomposeWTM(const yunuGI::Matrix4x4& mat, yunuGI::Vector3& scale, yunuGI::Quaternion& rotation, yunuGI::Vector3& translation);
		}
	}
}