#pragma once
#include "YunutyEngine.h"
using namespace yunutyEngine;

class RTSTerrain : public yunutyEngine::Component
{
public:
    // walkable Node가 인접 노드들과 어우러져 삼각형을 이룰 수 있다면 그 삼각형 평면이 걸어다닐 수 있는 경로가 된다.
    class Node
    {
        bool walkable{false};
    };
    // tiles[x][y]에 해당하는 좌표는 
    vector<vector<Node>> tiles;
    float tileWidth;
    // RTSTerrain객체의 위치 기준에서 좌로 우로 일정 공간을 차지하는 타일셋을 만들고 초기화함.
    void Initialize(int row,int col, float tileWidth)
    {
        this->tileWidth = tileWidth;
        tiles.resize()
    }
};
