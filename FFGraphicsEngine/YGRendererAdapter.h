
#pragma once

#include "YunuGraphicsInterface.h"
#include "CameraManager.h"
#include "FFGraphicsEngine.h"
#include "FFCamera.h"

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
            //FFGraphicsEngine::Instance.Get().SetCameraInfo(0,)

            LightParams lightParams = {};
            LightInfo info;
            //info.lightType = 0;
            info.position = FF::FFVec4{ 0.f,0.f,0.f };
            info.direction = FF::FFVec4{ 1.f, 0.f, 1.f };
            info.color.diffuse = FF::FFVec4(1.0f, 1.0f, 1.0f, 0.f);
            info.color.ambient = FF::FFVec4(0.3f, 0.3f, 0.3f, 0.f);
            info.color.specular = FF::FFVec4(0.f, 0.f, 0.f, 0.f);
            lightParams.lightCount = 1;
            lightParams.lights[0] = info;

            FFGraphicsEngine::Instance.Get().SetLightData(&lightParams, sizeof(lightParams));

            // static mesh를 모두 렌더한다.
            for (auto each : StaticMeshAdapter::instances)
            {
                if (each->m_materials.empty())
                {
                    wstring emptyWstr{};
                    FFGraphicsEngine::Instance.Get().DrawMesh(
                        each->m_WTM,
                        each->m_mesh,
                        {},
                        emptyWstr,
                        0
                    );
                }
                else
                {
                    for (auto i = 0; i < each->m_materials.size(); i++)
                    {
                        FFGraphicsEngine::Instance.Get().DrawMesh(
                            each->m_WTM,
                            each->m_mesh,
                            each->m_materials[i],
                            each->m_materials[i].materialName,
                            i
                        );
                    }
                }
            }

            FFGraphicsEngine::Instance.Get().RenderEnd();
        };
        virtual void ResizeResolution(unsigned int width, unsigned int height)
        {
            FFGraphicsEngine::Instance.Get().ResizeWindow(width, height);
        };
        virtual void* QuerySharedOutputHandle() { return nullptr; }
    };
}
