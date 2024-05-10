#include "YunutyEngine.h"
#include "IYunuGI3DRenderer.h"
#include <iostream>
#include <chrono>
#include <stack>
#include "YunutyCycle.h"
#include "YunutyTime.h"
#include "GameObject.h"
#include "Component.h"
#include "GlobalComponent.h"
#include "Scene.h"
#include "Camera.h"
#include "_YunutyPhysxGlobal.h"
#include <thread>
#include <mutex>
#include <cassert>


using namespace yunutyEngine;

yunutyEngine::YunutyCycle* yunutyEngine::YunutyCycle::_instance = nullptr;
yunutyEngine::YunutyCycle::YunutyCycle()
{
    assert(!_instance);
    _instance = this;
}
yunutyEngine::YunutyCycle::~YunutyCycle()
{
    Release();
}
void yunutyEngine::YunutyCycle::Release()
{
    Stop();
    GlobalComponent::globalComponents.clear();
    for (auto& each : GlobalComponent::globalComponents)
        delete each;
}
yunutyEngine::YunutyCycle& yunutyEngine::YunutyCycle::SingleInstance()
{
    if (!_instance)
        _instance = new YunutyCycle();
    return *_instance;
}
void yunutyEngine::YunutyCycle::Initialize()
{

}


void yunutyEngine::YunutyCycle::Play()
{
    isGameRunning = true;
    ActiveComponentsDo(StartComponent);
    yunutyEngine::physics::_PhysxGlobal::SingleInstance();
    updateThread = thread(&YunutyCycle::ThreadFunction, this);
}

void yunutyEngine::YunutyCycle::Stop()
{
    isGameRunning = false;
    if (updateThread.joinable())
        updateThread.join();
}

void yunutyEngine::YunutyCycle::Pause()
{
}

void yunutyEngine::YunutyCycle::SetMaxFrameRate()
{
}

bool yunutyEngine::YunutyCycle::IsGameRunning()
{
    return isGameRunning;
}
bool yunutyEngine::YunutyCycle::IsUpdating()
{
    return isUpdating;
}

void yunutyEngine::YunutyCycle::ThreadFunction()
{
    if (preThreadAction)
        preThreadAction();
    while (isGameRunning)
    {
        {
            //std::unique_lock lock{ preUpdateMutex };
            isUpdating = true;
            ThreadUpdate();
            isUpdating = false;
        }
        //updateMutexCV.notify_all();

        auto sleepImplied = 10;
        sleepImplied -= Time::GetDeltaTimeUnscaled() * 1000;
        /*   if (sleepImplied > 1)
               std::this_thread::sleep_for(std::chrono::milliseconds(sleepImplied));
       */
    }
    if (postThreadAction)
        postThreadAction();
}
void yunutyEngine::YunutyCycle::ResetUpdateTargetComponents()
{
    if (updateTargetComponents.size() < Component::guidPtrMap.size())
        updateTargetComponents.resize(Component::guidPtrMap.size() * 2);
    updateTargetComponentsSize = 0;

    static std::stack<GameObject*> gameObjectStack;
    for (auto eachGameObject : Scene::getCurrentScene()->GetUpdatingChildren())
        gameObjectStack.push(eachGameObject);
    while (!gameObjectStack.empty())
    {
        GameObject* gameObject = gameObjectStack.top();
        gameObjectStack.pop();
        for (auto each : gameObject->GetUpdatingChildren())
            gameObjectStack.push(each);
        for (auto eachComp : gameObject->updatingComponents)
            updateTargetComponents[updateTargetComponentsSize++] = eachComp;
    }
}

// Update components and render camera
void yunutyEngine::YunutyCycle::ThreadUpdate()
{
    std::scoped_lock lock{ updateMutex };
    Time::Update();

    for (auto i = GlobalComponent::globalComponents.begin(); i != GlobalComponent::globalComponents.end(); i++)
        (*i)->Update();

    assert(Scene::getCurrentScene() != nullptr && "Main Scene을 하나 생성해야 합니다!");
    // 살생부에 올라온 게임오브젝트들을 파괴합니다.
    while (!Scene::getCurrentScene()->destroyList.empty())
    {
        static std::vector<GameObject*> vector;
        vector.clear();
        vector = std::vector<GameObject*>(Scene::getCurrentScene()->destroyList.begin(), Scene::getCurrentScene()->destroyList.end());
        std::sort(vector.begin(), vector.end(), [](GameObject* a, GameObject* b) {return a->GetAncestorNumber() > b->GetAncestorNumber(); });

        for (auto each : vector)
        {
            for (auto each : each->GetIndexedComponents())
            {
                each->OnDestroy();
            }
            // 컴포넌트를 우측값으로 만들고 이를 다른 값에 할당하지 않으면 자연스레 할당해제된다.
            each->parent->MoveChild(each);
        }
        for (auto each : vector)
        {
            Scene::getCurrentScene()->destroyList.erase(each);
        }
    }

    {
        auto start = std::chrono::high_resolution_clock::now();

        ResetUpdateTargetComponents();
        for (unsigned int i = 0; i < updateTargetComponentsSize; i++)
            StartComponent(updateTargetComponents[i]);
        for (unsigned int i = 0; i < updateTargetComponentsSize; i++)
            UpdateComponent(updateTargetComponents[i]);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        Time::timeUsedForUpdate = duration.count() / 1000000000.0;
    }

    auto pxScene = physics::_PhysxGlobal::SingleInstance().RequestPxScene(Scene::currentScene);
    if (Time::GetDeltaTime() > 0 && pxScene)
    {
        // Start timing
        // 장식물에서 충돌체 다 뺄 경우 0.0005초정도 걸림
        auto start = std::chrono::high_resolution_clock::now();

        pxScene->simulate(Time::GetDeltaTime());
        pxScene->fetchResults(true);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        Time::timeUsedForPhysx = duration.count() / 1000000000.0;
        // Output the duration in milliseconds
        //std::cout << "Function took " << a << " milliseconds to execute.\n";
    }

    Collider2D::InvokeCollisionEvents();
    graphics::Renderer::SingleInstance().Update(Time::GetDeltaTime());

    if (postUpdateAction)
        postUpdateAction();

    if (autoRendering)
    {
        auto start = std::chrono::high_resolution_clock::now();

        graphics::Renderer::SingleInstance().Render();

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        Time::timeUsedForRender = duration.count() / 1000000000.0;
    }

    {
        std::scoped_lock lock(actionReservationMutex);
        for (auto each : afterUpdateActions)
            each();
        afterUpdateActions.clear();
    }
}

void yunutyEngine::YunutyCycle::ReserveActionAfterUpdate(std::function<void()> action)
{
    std::scoped_lock lock(actionReservationMutex);
    //actionReservationMutex.lock();
    afterUpdateActions.push_back(action);
    //actionReservationMutex.unlock();
}
void yunutyEngine::YunutyCycle::UpdateComponent(Component* component)
{
    if (!component->StartCalled)
    {
        component->StartCalled = true;
        component->Start();
    }
    component->Update();
}
void yunutyEngine::YunutyCycle::StartComponent(Component* component)
{
    if (component->StartCalled)
        return;
    component->StartCalled = true;
    component->Start();
}
void yunutyEngine::YunutyCycle::ActiveComponentsDo(function<void(Component*)> todo)
{
    for (auto each : updateTargetComponents)
        todo(each);
}
void yunutyEngine::YunutyCycle::ActiveComponentsDo(void (Component::* method)())
{
    for (auto each : updateTargetComponents)
        (each->*method)();
}
vector<Component*> yunutyEngine::YunutyCycle::GetActiveComponents()
{
    vector<Component*> ret;
    for (auto eachGameObj : GetGameObjects())
        for (auto component = eachGameObj->components.begin(); component != eachGameObj->components.end(); component++)
            if (component->first->GetActive())
                ret.push_back(component->first);
    return ret;
}
vector<GameObject*> yunutyEngine::YunutyCycle::GetGameObjects(bool onlyActive)
{
    vector<GameObject*> ret;
    yunutyEngine::IGameObjectParent* gameObjectParent = Scene::getCurrentScene();
    if (Scene::getCurrentScene())
    {
        stack<GameObject*> objectsStack;
        auto firstChildren = Scene::getCurrentScene()->GetChildren();
        for (auto each = firstChildren.rbegin(); each != firstChildren.rend(); each++)
            objectsStack.push(*each);

        while (!objectsStack.empty())
        {
            auto gameObject = objectsStack.top();
            objectsStack.pop();
            if (onlyActive && !gameObject->GetSelfActive())
                continue;

            ret.push_back(gameObject);
            for (auto each = gameObject->children.begin(); each != gameObject->children.end(); each++)
                objectsStack.push(each->first);
        }
    }
    return ret;
}
