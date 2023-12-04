/// 2023. 11. 21 김상준
/// 파일 형태로 저장하기 위한 json 을 다루는 클래스
/// 해당 클래스를 상속받아 Pre / Post 입출력을 재정의하여 구체화함

#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Application
{
	namespace Editor
	{
		class Storable
		{
		public:
			json Encoding() const;
			bool Decoding(const json& data);

		protected:
			virtual bool PreEncoding(json& data) const = 0;
			virtual bool PostEncoding(json& data) const = 0;
			virtual bool PreDecoding(const json& data) = 0;
			virtual bool PostDecoding(const json& data) = 0;
		};
	}
}

