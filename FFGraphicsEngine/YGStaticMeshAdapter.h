#pragma once
#include <unordered_set>
#include "YunuGraphicsInterface.h"
#include "IYunuGIRenderable.h"
#include "YGRenderableAdapter.h"

namespace yunuGIAdapter
{
    class StaticMeshAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::IStaticMeshRenderer
    {
    public:
        static std::unordered_set<StaticMeshAdapter*> instances;
        FF::FFMatrix m_WTM;
        std::wstring m_mesh; 
        FF::FFMATERIAL_DESC m_material;
        std::vector<FF::FFMATERIAL_DESC> m_materials;
        StaticMeshAdapter() :RenderableAdapter() { instances.insert(this); }
        ~StaticMeshAdapter() { instances.insert(this); }
        virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
        {
        };
        virtual void SetActive(bool isActive) {};
        virtual void LoadMesh(const char* fileName) {};
        virtual void LoadDiffuseMap(const char* fileName) {}
        virtual void LoadNormalMap(const char* fileName) {}
        virtual void SetPickingMode(bool isPickingModeOn) {}
    private:
    };
}
