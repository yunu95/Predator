#pragma once
#include "Object.h"
#include "Component.h"
#include "Rect.h"
#include "YunuGraphicsInterface.h"

// 카메라는 화면에 게임상에 존재하는 모든 그래픽 요소들을 출력하는 데에 쓰이는 객체입니다. 
// 카메라의 구현은 엔진의 동작환경,혹은 카메라의 특성에 따라 달라질 수 있기 때문에,
// 화면을 그려내기 위해 쓰이는 함수 Render는 추상 메소드로 정의됩니다.
#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine::graphics
{
    class YunutyCycle;
    class YUNUTY_API Camera : public Component
    {
    private:
        yunuGI::GIPtr<yunuGI::ICamera> yunuGICamera;
        static Camera* mainCamera;
    protected:
        //virtual void Render() = 0;
        virtual void Update() override;
    public:
        Camera();
        void SetCameraMain();
        yunuGI::ICamera& GetGI();
        /// <summary>
        /// 화면의 중심이 {0,0} 상단이 0.5, 하단이 -0.5인 좌단이 -0.5, 우단이 0.5인 스크린 좌표계에서 좌표를 받아 이를 월드 좌표계로 변환해 반환합니다.
        /// 스크린 스페이스의 좌표는 카메라의 near plane으로부터 distance만큼 떨어져 있는 평면에 사영됩니다. 
        /// </summary>
        /// <param name="normalizedScreenPosition"></param>
        /// <returns></returns>
        virtual Vector3d GetProjectedPoint(const Vector2d& normalizedScreenPosition, double distance);

        /// <summary>
        ///  화면의 중심이 {0,0} 상단이 0.5, 하단이 -0.5인 좌단이 -0.5, 우단이 0.5인 스크린 좌표계에서 좌표를 받아 이를 월드 좌표계로 변환해 반환합니다.
        /// 스크린 스페이스의 좌표는 카메라의 near plane으로부터 distance만큼 떨어져 있고, normal vector 의 방향이 norm 인 평면에 사영됩니다.
        /// </summary>
        /// <param name="normalizedScreenPosition"></param>
        /// <param name="distance"></param>
        /// <returns></returns>
        virtual Vector3d GetProjectedPoint(const Vector2d& normalizedScreenPosition, double distance, const Vector3d& norm);
        static Camera* GetMainCamera();
        friend YunutyCycle;
    };
}
