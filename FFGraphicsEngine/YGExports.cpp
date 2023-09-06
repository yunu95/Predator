#include "GraphicsPch.h"
#include "YunuGraphicsInterface.h"
#include "YGAdapter.h"
#include "YGFactoryAdapter.h"
#include "YGResourceManagerAdapter.h"
#include "YGEventHandlerAdapter.h"
#include "YunuGIGlobal.h"

//#include "YunuGIRendererAdapter.h"

extern "C"
{
    __declspec(dllexport) yunuGI::I3DRenderer* CreateRenderer()
    {
        return new yunuGIAdapter::RendererAdapter();
    }
    __declspec(dllexport) yunuGI::IFactory* CreateFactory()
    {
        return new yunuGIAdapter::FactoryAdapter();
    }
    __declspec(dllexport) yunuGI::IEventHandler* CreateEventHandler()
    {
        return new yunuGIAdapter::EventHandlerAdapter();
    }
    __declspec(dllexport) yunuGI::IResourceManager* CreateResourceManager()
    {
        return new yunuGIAdapter::ResourceManagerAdapter();
    }
    __declspec(dllexport) void ReleaseYunuGI(yunuGI::IUnknown* target)
    {
        delete target;
    }
}