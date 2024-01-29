#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class TerrainPalette : public Palette, public  yunutyEngine::SingletonClass<TerrainPalette>
            {
            public:
                virtual IEditableData* PlaceInstance(Vector3d worldPosition)override;
                virtual void OnMouseMove(Vector3d projectedWorldPos)override;
                virtual void SetAsSelectMode(bool isSelectMode) override;
                // isMarking은 지형을 추가하고 있는지 없애고 있는지 나타냅니다. 
                void ApplyAsPlaytimeObjects();
                //virtual void CleanUpPlaytimeObjects() override {};
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
                bool IsMarking();
                void SetIsMarking(bool isMarking);
                // 브러시 크기는 0부터 시작합니다.
                unsigned int GetBrushSize();
                void SetBrushSize(unsigned int brushSize);
                void AddNode(const Vector2i& nodeKey);
                void EraseNode(const Vector2i& nodeKey);
                void ClearNodes();
                vector<Vector2i> GetNodePositionList();
                static constexpr double nodeDistance = 0.5;
                static constexpr double nodeHeight = 6;
            private:
                int brushSize = 0;
                bool isMarking{ true };
                unordered_map<Vector2i, GameObject*> nodes;
                Vector3d GetNodePosition(const Vector2i& nodeKey);
                Vector2i WorldToNodeSpace(const Vector3d& worldPos);
                GameObject* CreateNodeDebuggingMesh(const Vector2i& nodeKey);
            };
        }
    }
}
