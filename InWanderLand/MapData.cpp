#pragma once
#include "MapData.h"
#include "TerrainPalette.h"
#include "Terrain_TemplateData.h"

string MapData::currentFileName;
application::editor::TemplateDataManager& MapData::tdm = application::editor::TemplateDataManager::GetSingletonInstance();
application::editor::MapFileManager& MapData::mfm = application::editor::MapFileManager::GetSingletonInstance();
application::editor::InstanceManager& MapData::im = application::editor::InstanceManager::GetSingletonInstance();

void MapData::ApplyMapData()
{
    using namespace application::editor::palette;
    TerrainPalette::SingleInstance().ApplyAsPlaytimeObjects();
}
void MapData::CleanUpAppliedObjects()
{

}
void MapData::LoadMapData(const string& fileName)
{
    currentFileName = fileName;
    CleanMapData();
    mfm.LoadMapFile(fileName);
    LoadTerrainData();
}
void MapData::SaveMapData(const string& fileName)
{
    currentFileName = fileName;
    SaveTerrainData();
}
void MapData::GetCurrentFileName()
{

}
void MapData::LoadTerrainData()
{
    using namespace application::editor::palette;
    using namespace application::editor;
    auto terrainTemplateData = tdm.GetTemplateData<Terrain_TemplateData>("TerrainData");
    for (auto eachCoord : terrainTemplateData->pod.coordinates)
    {
        TerrainPalette::SingleInstance().AddNode({ eachCoord.first,eachCoord.second });
    }
}
void MapData::SaveTerrainData()
{
    using namespace application::editor::palette;
    using namespace application::editor;
    auto terrainTemplateData = tdm.GetTemplateData<Terrain_TemplateData>("TerrainData");
    if (terrainTemplateData == nullptr)
        terrainTemplateData = tdm.CreateTemplateData<Terrain_TemplateData>("TerrainData");
    terrainTemplateData->pod.coordinates.clear();
    for (auto eachCoord : TerrainPalette::SingleInstance().GetNodePositionList())
    {
        terrainTemplateData->pod.coordinates.push_back({ eachCoord.x, eachCoord.y });
    }
}
void MapData::CleanMapData()
{
    using namespace application::editor::palette;
    using namespace application::editor;
    TerrainPalette::SingleInstance().ClearNodes();
}
