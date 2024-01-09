#include "EventManager.h"

namespace application
{
    namespace editor
    {
        bool EventManager::PushEventCallable(std::function<void()> eventFunc)
        {
            eventFuncQueue.push(eventFunc);
            return true;
        }

        std::function<void()> EventManager::PopEventCallable()
        {
            auto func = eventFuncQueue.front();
            eventFuncQueue.pop();
            return func;
        }

        size_t EventManager::Size()
        {
            return eventFuncQueue.size();
        }

        void EventManager::Clear()
        {
            while (eventFuncQueue.size() != 0)
            {
                eventFuncQueue.pop();
            }
        }
    }
}