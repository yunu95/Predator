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

        virtual void SetOutputWindow(UINT64 hWnd)
        {
            NailEngine::Instance.Get().Init(hWnd);
        };

        virtual void Update(float deltaTime) { };

        virtual void Render() 
        {
            NailEngine::Instance.Get().Render();
        };

        virtual void ResizeResolution(unsigned int width, unsigned int height) 
        {
            NailEngine::Instance.Get().SetResolution(width, height);
        };

        virtual void* QuerySharedOutputHandle() { return nullptr; }
    };
}
