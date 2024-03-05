/// 2024. 03. 04 김상준
/// filesystem 을 활용하여 윈도우용 파일 입출력 구현

#pragma once
#include <filesystem>

namespace application
{
	namespace editor
	{
		namespace fileSystem
		{
			std::filesystem::path SaveFileDialog(const char* filter = "All\0*.*\0", const char* initialDir = "");
			std::filesystem::path LoadFileDialog(const char* filter = "All\0*.*\0", const char* initialDir = "");
		};
	}
}
