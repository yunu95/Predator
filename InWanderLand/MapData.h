#pragma once
#include "YunutyEngine.h"

class MapData
{
public:
    // 로드된 맵 데이터로부터 인게임 객체들을 생성합니다.
    static void ApplyMapData();
    // 인게임 객체들을 모두 삭제합니다.
    static void CleanUpAppliedObjects();
    // 파일로부터 맵 데이터를 로드합니다.
    // EDITOR 전처리기 지시문이 활성화된 경우 편집기용 객체들을 생성합니다.
    static void LoadMapData(const wstring& fileName);
    // 현재 프로세스에서 편집된 맵 데이터를 파일로 저장합니다.
    static void SaveMapData(const wstring& fileName);
    // 현재 로드된 맵 파일의 이름을 반환합니다.
    static void GetCurrentFileName();
private:
    // 현재 로드된 맵 데이터들을 모두 삭제합니다.
    static void CleanMapData();
    static wstring currentFileName;
};
