#pragma once
#include "YunuGraphicsInterface.h"

#include "NailEngine.h"

namespace yunuGIAdapter
{
    class RendererAdapter : public yunuGI::I3DRenderer
    {
    public:
        RendererAdapter() { };
        virtual ~RendererAdapter() 
        {
            NailEngine::Instance.Release();
        }

        virtual void SortByCameraDirection()
        {
            InstancingManager::Instance.Get().SortByCameraDirection();
        };

		virtual void SetUseIBL(bool useIBL) override
        {
            NailEngine::Instance.Get().SetUseIBL(useIBL);
        }

		virtual void SetLightMap(const std::wstring& lightMapName) override
        {
            NailEngine::Instance.Get().SetLightMap(lightMapName);
        }

        virtual void SetOutputWindow(UINT64 hWnd)
        {
            NailEngine::Instance.Get().Init(hWnd);
        };

        virtual void Update(float deltaTime) { };

        virtual void Render() 
        {
            NailEngine::Instance.Get().Render();
        };
        virtual void SetResolution(unsigned int width, unsigned int height)
        {
            NailEngine::Instance.Get().SetResolution(width, height);
        };
        virtual void ResizeResolution(unsigned int width, unsigned int height) 
        {
            NailEngine::Instance.Get().ResizeResolution(width, height);
        };

        virtual void* QuerySharedOutputHandle() { return nullptr; }

        virtual void Finalize()
        {
            NailEngine::Instance.Get().Finalize();
        };

        virtual void SetUseLightMap(bool useLightMap) override
        {
            NailEngine::Instance.Get().SetUseLightMap(useLightMap);
        };

        virtual void SetClearColor(yunuGI::Color color) override
        {
            NailEngine::Instance.Get().SetClearColor(color);
        };
    };
}
