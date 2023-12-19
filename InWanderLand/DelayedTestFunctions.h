#pragma once
#include "YunutyEngine.h"
#include <algorithm>

class DelayedTestFunctions : public yunutyEngine::Component
{
public:
    struct Todo
    {
        double timeoffset;
        function<void()> todo;
        bool operator<(const Todo& other) const
        {
            return timeoffset < other.timeoffset;
        }
    };
    vector<Todo> todoList;
protected:
    virtual void Start()override
    {
        sort(todoList.begin(), todoList.end(), std::less<>());
    }
    virtual void Update()override
    {
        if (todoIdx >= todoList.size())
            return;

        timeElapsed += Time::GetDeltaTime();
        if (todoList[todoIdx].timeoffset < timeElapsed)
        {
            todoList[todoIdx].todo();
            todoIdx++;
        }
    }
private:
    double timeElapsed{0};
    int todoIdx{ 0 };
};
