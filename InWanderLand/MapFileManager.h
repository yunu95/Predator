/// 2023. 11. 21 김상준
/// Map File 입출력을 위한 클래스

#pragma once

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
		{
		public:
			static MapFileManager& GetInstance();

			virtual ~MapFileManager();

			bool LoadMapFile(const std::string& path);
			bool SaveMapFile(const std::string& path);

			void Clear();

		private:
			static std::unique_ptr<MapFileManager> instance;

			MapFileManager();
			MapFileManager(const MapFileManager&) = delete;
			MapFileManager& operator=(const MapFileManager&) = delete;

			InstanceManager& instanceManager;
			TemplateDataManager& templateDataManager;

			std::string currentMap;
		};
	}
}
