/// 2023. 11. 21 김상준
/// 파일 형태로 저장하기 위한 json 을 다루는 클래스
/// 해당 클래스를 상속받아 Pre / Post 입출력을 재정의하여 구체화함

#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <boost/pfr.hpp>

using json = nlohmann::json;

namespace Application
{
	namespace Editor
	{	
		template <int N, typename T>
		json FieldEncoding(T& className, json& data)
		{
			if constexpr (N == 0)
			{
				return data;
			}
			else
			{
				data[boost::pfr::get_name<N - 1, T>()] = boost::pfr::get<N - 1>(className);
				FieldEncoding<N - 1, T>(className, data);
			}
			return data;
		}

		class Storable
		{
		public:
			// json 형태로 Storable 클래스를 저장합니다.
			json Encoding() const;
			// json 형태로부터 Storable 클래스를 초기화합니다.
			bool Decoding(const json& data);

		protected:
			virtual bool PreEncoding(json& data) const = 0;
			virtual bool PostEncoding(json& data) const = 0;
			virtual bool PreDecoding(const json& data) = 0;
			virtual bool PostDecoding(const json& data) = 0;
		};
	}
}

#define TO_JSON(Class) \
operator json() \
{ \
	json data; \
	Class classObj = Class(); \
	Application::Editor::FieldEncoding<boost::pfr::tuple_size_v<Class>>(classObj, data); \
	return data; \
}
