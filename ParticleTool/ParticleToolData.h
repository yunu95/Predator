/// 2024. 04. 23 김상준
/// Particle 제작에 대한 데이터를 저장할 클래스

#pragma once

#include "YunutyEngine.h"

#include <string>
#include <unordered_map>
#include <string>
#include <vector>
#include <type_traits>
#include <nlohmann/json.hpp>
#include <boost/pfr.hpp>

using json = nlohmann::json;

// STL Container 확인용 템플릿 클래스
template <typename T, typename = void>
class is_STL_Container
	: public std::false_type
{

};

// void 특수화, begin() / end() 함수 유무로만 판단함
// 예외에 대해서는 우선 생각하지 않음
template <typename T>
struct is_STL_Container<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
	: std::true_type
{

};

namespace application
{
	namespace particle
	{
		template <int N, typename T>
		json FieldPreEncoding(T& classInstance, json& data)
		{
			if constexpr (N == 0)
			{
				return data;
			}
			else
			{
				// Not Pointer, nlohmann json 은 STL Container 변환을 지원함
				if constexpr (!std::is_pointer_v<std::remove_reference_t<decltype(boost::pfr::get<N - 1>(classInstance))>>)
				{
					data[boost::pfr::get_name<N - 1, std::remove_const_t<T>>()] = boost::pfr::get<N - 1>(classInstance);
				}
				FieldPreEncoding<N - 1, T>(classInstance, data);
			}
			return data;
		}

		template <int N, typename T>
		const json& FieldPreDecoding(T& classInstance, const json& data)
		{
			if constexpr (N == 0)
			{
				return data;
			}
			else
			{
				if (!data.contains(boost::pfr::get_name<N - 1, T>()))
				{
					FieldPreDecoding<N - 1, T>(classInstance, data);
					return data;
				}

				// STL Container
				else if constexpr (is_STL_Container<std::remove_reference_t<decltype(boost::pfr::get<N - 1>(classInstance))>>::value)
				{
					boost::pfr::get<N - 1>(classInstance) = data[boost::pfr::get_name<N - 1, T>()].get<std::remove_reference_t<decltype(boost::pfr::get<N - 1>(classInstance))>>();
				}
				else
				{
					boost::pfr::get<N - 1>(classInstance) = data[boost::pfr::get_name<N - 1, T>()];
				}
				FieldPreDecoding<N - 1, T>(classInstance, data);
			}
			return data;
		}
	}
}

#define TO_JSON(Class) \
operator json() const \
{ \
	json data; \
	application::particle::FieldPreEncoding<boost::pfr::tuple_size_v<Class>>(*this, data); \
	return data; \
}

#define FROM_JSON(Class) \
Class& operator=(const json& data) \
{ \
	application::particle::FieldPreDecoding<boost::pfr::tuple_size_v<Class>>(*this, data); \
	return *this; \
}

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
            ParticleShape shape = ParticleShape::Cone;
            ParticleMode particleMode = ParticleMode::Default;
            bool isLoop = false;
            float lifeTime = 5.f;
            float speed = 1.f;
            float startScale = 1.f;
            float endScale = 1.f;
            unsigned int maxParticle = 500;
            bool playAwake = true;

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
