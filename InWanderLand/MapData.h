#pragma once
#include "YunutyEngine.h"
#include "TemplateDataManager.h"
#include "MapFileManager.h"
#include "InstanceManager.h"

class MapData
{
public:
    // 로드된 맵 데이터로부터 인게임 객체들을 생성합니다.
    static void ApplyMapData();
    // 인게임 객체들을 모두 삭제합니다.
    static void CleanUpAppliedObjects();
    // 파일로부터 맵 데이터를 로드합니다.
    // EDITOR 전처리기 지시문이 활성화된 경우 편집기용 객체들을 생성합니다.
    static void LoadMapData(const string& fileName);
    // 현재 프로세스에서 편집된 맵 데이터를 파일로 저장합니다.
    static void SaveMapData(const string& fileName);
    // 현재 로드된 맵 파일의 이름을 반환합니다.
    static void GetCurrentFileName();
private:
    // 지형, 유닛, 장식물, 지역 등의 객체들을 데이터 형태로 저장하거나 데이터 형태에서 불러옵니다.
    static void LoadTerrainData();
    static void SaveTerrainData();
    static void LoadUnitInstancesData();
    static void SaveUnitInstancesData();
    // 현재 로드된 맵 데이터들을 모두 삭제합니다.
    static void CleanMapData();
    static string currentFileName;
    static application::editor::TemplateDataManager& tdm;
    static application::editor::MapFileManager& mfm;
    static application::editor::InstanceManager& im;
};
