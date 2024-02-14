/// 2023. 11. 21 김상준
/// 파일 형태로 저장하기 위한 json 을 다루는 클래스
/// 해당 클래스를 상속받아 Pre / Post 입출력을 재정의하여 구체화함

#pragma once

#include "Identifiable.h"
#include "UUIDManager.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <type_traits>
#include <nlohmann/json.hpp>
#include <boost/pfr.hpp>

using json = nlohmann::json;

namespace application
{
	namespace editor
	{
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
		json FieldPostEncoding(T& classInstance, json& data)
		{
			if constexpr (N == 0)
			{
				return data;
			}
			else
			{
				// Pointer
				if constexpr (std::is_pointer_v<std::remove_reference_t<decltype(boost::pfr::get<N - 1>(classInstance))>>)
				{
					data[boost::pfr::get_name<N - 1, std::remove_const_t<T>>()] = UUID_To_String(boost::pfr::get<N - 1>(classInstance)->GetUUID());
				}
				FieldPostEncoding<N - 1, T>(classInstance, data);
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
				// pointer
				if constexpr (std::is_pointer_v<std::remove_reference_t<decltype(boost::pfr::get<N - 1>(classInstance))>>)
				{
					boost::pfr::get<N - 1>(classInstance) = nullptr;
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

		template <int N, typename T>
		const json& FieldPostDecoding(T& classInstance, const json& data)
		{
			if constexpr (N == 0)
			{
				return data;
			}
			else
			{
				// pointer
				if constexpr (std::is_pointer_v<std::remove_reference_t<decltype(boost::pfr::get<N - 1>(classInstance))>>)
				{
					boost::pfr::get<N - 1>(classInstance) = UUIDManager::GetSingletonInstance()
						.GetPointerFromUUID<std::remove_reference_t<decltype(boost::pfr::get<N - 1>(classInstance))>>
						(String_To_UUID(data[boost::pfr::get_name<N - 1, T>()]));
				}
				FieldPostDecoding<N - 1, T>(classInstance, data);
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
operator json() const \
{ \
	json data; \
	application::editor::FieldPreEncoding<boost::pfr::tuple_size_v<Class>>(*this, data); \
	return data; \
}

#define FROM_JSON(Class) \
Class& operator=(const json& data) \
{ \
	application::editor::FieldPreDecoding<boost::pfr::tuple_size_v<Class>>(*this, data); \
	return *this; \
}
