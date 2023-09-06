#pragma once
#include "YunuGraphicsInterface.h"
#include "FFGraphicsEngine.h"

namespace yunuGIAdapter
{
    class RendererAdapter : public yunuGI::I3DRenderer
    {
    public:
        float deltaTime;
        RendererAdapter() { };

        virtual ~RendererAdapter() {}

        virtual void SetOutputWindow(UINT64 hWnd)
        {
            FFGraphicsEngine::Instance.Get().Init(static_cast<UINT64>(hWnd), 1920.f, 1080.f, true);
        };

        virtual void Update(float deltaTime)
        {
            this->deltaTime = deltaTime;
        };
        virtual void Render()
        {
            FFGraphicsEngine::Instance.Get().RenderBegin(this->deltaTime);

			LightParams lightParams = {};
			LightInfo info;
			info.position = FF::FFVec4{ 0.f,0.f,0.f };
			info.direction = FF::FFVec4{ 1.f, 0.f, 1.f };
			info.color.diffuse = FF::FFVec4(1.0f, 1.0f, 1.0f, 0.f);
			info.color.ambient = FF::FFVec4(0.3f, 0.3f, 0.3f, 0.f);
			info.color.specular = FF::FFVec4(0.f, 0.f, 0.f, 0.f);
			lightParams.lightCount = 1;
			lightParams.lights[0] = info;

            FFGraphicsEngine::Instance.Get().SetLightData(&lightParams, sizeof(lightParams));

            FFGraphicsEngine::Instance.Get().RenderEnd();
        };
        virtual void ResizeResolution(unsigned int width, unsigned int height) {};
        virtual void* QuerySharedOutputHandle() { return nullptr; }
    };
}
