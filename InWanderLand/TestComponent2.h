#pragma once
#include "YunutyEngine.h"

class TestComponent2 : public yunutyEngine::Component
{
public:
    GameObject* gameObject;

    void Update()
    {
        if (Input::isKeyDown(KeyCode::H))
        {
            if (gameObject)
            {
                yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(gameObject);
                gameObject = nullptr;
            }
            //gameObject->SetSelfActive(false);
        }
        if (Input::isKeyDown(KeyCode::J))
        {
            gameObject->SetSelfActive(true);
        }
    }
};

