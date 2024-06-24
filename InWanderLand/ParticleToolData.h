/// 2024. 04. 23 김상준
/// Particle 제작에 대한 데이터를 저장할 클래스

#pragma once

#include "YunutyEngine.h"

#include "Storable.h"

#include <string>
#include <unordered_map>
#include <string>
#include <vector>
#include <type_traits>
#include <nlohmann/json.hpp>
#include <boost/pfr.hpp>

namespace application
{
    namespace particle
    {
        enum class ParticleShape
        {
            Cone,
            Circle,
        };

        enum class ParticleMode
        {
            Default,
            Bursts,
        };

        struct ParticleToolData
        {
        public:
            /// 저장할 데이터들
            std::string name = "None";
            std::string texturePath = "Texture/Particle/default.dds";
            ParticleShape shape = ParticleShape::Cone;
            ParticleMode particleMode = ParticleMode::Default;
            bool isLoop = false;
            float duration = 5.0;
            float lifeTime = 5.f;
            float speed = 1.f;
            float startScale = 1.f;
            float endScale = 1.f;
            unsigned int maxParticle = 500;
            bool playAwake = true;
            bool isApplyRoot = true;

            /// Cone
            float radius = 1.f;
            float angle = 25.f;

            /// Default
            float rateOverTime = 10.f;

            /// Bursts
            int burstsCount = 30;
            float interval = 5.f;

            TO_JSON(ParticleToolData)
            FROM_JSON(ParticleToolData)
        };

		struct ParticleToolInstance
        {
        public:
			ParticleToolData particleData = ParticleToolData();

			std::string targetUnit = "None";
			std::string name = "None";
            Vector3d offsetPos = Vector3d();
            Quaternion rotation = Quaternion();
            Vector3d scale = Vector3d(1, 1, 1);
        };
    }
}
