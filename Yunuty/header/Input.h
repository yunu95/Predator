#pragma once
#include "Object.h"
#include "EKeycode.h"
#include "GlobalComponent.h"
#include "Vector2.h"
#include <dinput.h>
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Input.h"
#include "Vector2.h"
#include "Rect.h"


#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
namespace yunutyEngine
{
    namespace graphics
    {
        class Renderer;
    }
    class YUNUTY_API Input :
        public GlobalComponent
    {
    public:
        // 사용자가 키를 누르고 있는가?
        static bool isKeyDown(KeyCode keyCode);
        // 해당 프레임에 키가 갓 눌렸는가?
        static bool isKeyPushed(KeyCode keyCode);
        // 해당 프레임에 키가 갓 떼어졌는가?
        static bool isKeyLifted(KeyCode keyCode);
        // 인풋 시스템을 활성화 / 비활성화합니다. 게임 윈도우 창이 포커스되거나 포커스를 잃었을 때 알맞게 호출하십시오.
        static void SetActive(bool active);
        static bool IsActive();
        static Vector2d getMouseScreenPosition();
        static Vector2d getMouseScreenPositionNormalized();
        // 마우스의 스크린 좌표를 -0.5~0.5 사이의 값으로 정규화하여 반환합니다.
        // x축의 경우 왼쪽이 -0.5, 오른쪽이 0.5입니다.
        // y축의 경우 아래쪽이 -0.5, 위쪽이 0.5입니다.
        static Vector2d getMouseScreenPositionNormalizedZeroCenter();
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    protected:
        virtual void Update()override;
    private:
        static Input* instance;
        static Input* GetInstance();
        unordered_set<KeyCode> keyDownSet;
        unordered_set<KeyCode> keyPushedSet;
        unordered_set<KeyCode> keyLiftedSet;
        unordered_map<unsigned int, KeyCode> rawKeyEnumMap;
        Vector2d cachedMouseScreenPosition;
        Input();
        bool m_isKeyDown(KeyCode keyCode);
        bool m_isKeyPushed(KeyCode keyCode);
        bool m_isKeyLifted(KeyCode keyCode);
        void CacheMouseScreenPosition();
        virtual ~Input();
        HWND mainWnd{ 0 };
        bool isActive{ true };

        friend graphics::Renderer;
    };
}
