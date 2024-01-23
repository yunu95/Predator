/// 2024. 01. 22 김상준
/// OpenGL 기반의 GLM 라이브러리를 지원하는 수학 계산 모듈
/// yunuGI 값으로의 변환을 지원함

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/constants.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "YunuGraphicsInterface.h"

namespace application
{
	namespace editor
	{
		namespace math
		{
			yunuGI::Vector3 ConvertVector3(const glm::vec3& vec);
			yunuGI::Vector4 ConvertVector4(const glm::vec4& vec);
			yunuGI::Matrix4x4 ConvertMatrix4x4(const glm::mat4& mat);
			yunuGI::Quaternion ConvertQuaternion(const glm::quat& quat);

			yunuGI::Matrix4x4 GetRotationMatrix(const yunuGI::Quaternion& quat);
			yunuGI::Matrix4x4 GetRotationMatrix(const glm::quat& quat);

			yunuGI::Vector3 RotateVector3(const yunuGI::Vector3& vec, const yunuGI::Matrix4x4& roMat);
		}
	}
}