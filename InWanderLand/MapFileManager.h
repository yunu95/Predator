/// 2023. 11. 21 김상준
/// Map File 입출력을 위한 클래스

#pragma once

#include "Singleton.h"

#include <string>
#include <memory>

namespace application
{
	namespace editor
	{
		class InstanceManager;
		class TemplateDataManager;
	}
}

namespace application
{
	namespace editor
	{
		class MapFileManager
			: public Singleton<MapFileManager>
		{
		public:
			MapFileManager();

			bool LoadMapFile(const std::string& path);
			bool SaveMapFile(const std::string& path);
			std::string GetCurrentMapPath() const;
			void SetCurrentMapPath(std::string& path);
			void Clear();

		private:
			InstanceManager& instanceManager;
			TemplateDataManager& templateDataManager;

			std::string currentMapPath;
		};
	}
}
