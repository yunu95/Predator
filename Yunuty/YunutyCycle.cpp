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

void yunutyEngine::YunutyCycle::HandleComponent(Component* component, bool forceDrop)
{
    if (forceDrop)
    {
        startTargetComponentsPendingDeletion.push(component);
        updateTargetComponentsPendingDeletion.push(component);
        return;
    }
    if (component->GetGameObject()->GetActive() && component->GetActive())
    {
        if (!component->wasStartCalled())
            startTargetComponentsPendingAddition.push(component);

        if (component->isUpdating)
            updateTargetComponentsPendingAddition.push(component);
        else
            updateTargetComponentsPendingDeletion.push(component);
    }
    else
    {
        startTargetComponentsPendingDeletion.push(component);
        updateTargetComponentsPendingDeletion.push(component);
    }

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
            {std::unique_lock lock{ preUpdateMutex }; }
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
void yunutyEngine::YunutyCycle::ReceivePendingObjectsChange()
{
    while (!updateTargetComponentsPendingAddition.empty())
    {
        updateTargetComponents.insert(updateTargetComponentsPendingAddition.top());
        updateTargetComponentsPendingAddition.pop();
    }
    while (!updateTargetComponentsPendingDeletion.empty())
    {
        updateTargetComponents.erase(updateTargetComponentsPendingDeletion.top());
        updateTargetComponentsPendingDeletion.pop();
    }
    while (!startTargetComponentsPendingAddition.empty())
    {
        startTargetComponents.insert(startTargetComponentsPendingAddition.top());
        startTargetComponentsPendingAddition.pop();
    }
    while (!startTargetComponentsPendingDeletion.empty())
    {
        startTargetComponents.erase(startTargetComponentsPendingDeletion.top());
        startTargetComponentsPendingDeletion.pop();
    }
}

// Update components and render camera
void yunutyEngine::YunutyCycle::ThreadUpdate()
{
    std::unique_lock lock{ updateMutex };
    try
    {
        Time::Update();

        for (auto i = GlobalComponent::globalComponents.begin(); i != GlobalComponent::globalComponents.end(); i++)
            (*i)->Update();

        // 살생부에 올라온 게임오브젝트들을 파괴합니다.
        for (auto each : Scene::getCurrentScene()->destroyList)
        {
            for (auto each : each->GetIndexedComponents())
                each->OnDestroy();
            each->parent->MoveChild(each);
        }
        Scene::getCurrentScene()->destroyList.clear();

        ReceivePendingObjectsChange();

        //// SetCacheDirty는 GetSceneIndex, 즉 게임오브젝트가 현재 씬에서 몇번째 객체인지 알아야 했을때 쓰였습니다.
        //for (auto each : gameObjects)
        //    each->SetCacheDirty();
        //for (auto each : GetGameObjects(false))
        //    each->SetCacheDirty();
        //for (auto each : GetActiveComponents())
        //    UpdateComponent(each);
        for (auto each : startTargetComponents)
            if (each->wasStartCalled() == false)
                StartComponent(each);
        startTargetComponents.clear();
        for (auto each : updateTargetComponents)
            UpdateComponent(each);


        auto pxScene = physics::_PhysxGlobal::SingleInstance().RequestPxScene(Scene::currentScene);
        if (Time::GetDeltaTime() > 0 && pxScene)
        {
            pxScene->simulate(Time::GetDeltaTime());
            pxScene->fetchResults(true);
        }

        Collider2D::InvokeCollisionEvents();
        graphics::Renderer::SingleInstance().Update(Time::GetDeltaTime());

        if (postUpdateAction)
            postUpdateAction();

        if (autoRendering)
        {
            graphics::Renderer::SingleInstance().Render();
        }

        {
            std::scoped_lock lock(actionReservationMutex);
            for (auto each : afterUpdateActions)
                each();
            afterUpdateActions.clear();
        }
    }
    catch (const std::exception& e)
    {
        if (onExceptionThrown)
            onExceptionThrown(e);
        else
            throw e;
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
