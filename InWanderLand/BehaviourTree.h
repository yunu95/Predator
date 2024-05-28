#include <vector>
#include <functional>
#include <unordered_set>
#include <unordered_map>

class BehaviourTree
{
public:
    struct Node;
    struct Node
    {
        std::unordered_map<int, Node> children;
        std::function<void()> onEnter = []() {};
        std::function<void()> onExit = []() {};
        std::function<void()> onUpdate = []() {};
        std::function<bool()> enteringCondtion = []() {return false; };
    };
    std::unordered_map<int, Node> rootChildren;
    std::queue<const Node*> activeOnLastUpdate;
    std::queue<const Node*> activeOnCurrentUpdate;
    //std::unordered_set<Node*> activeOnLastUpdate;
    //std::unordered_set<Node*> activeOnCurrentUpdate;
    void Update()
    {
        activeOnCurrentUpdate = std::queue<const Node*>{};
        const auto* nodeChildren = &rootChildren;
        while (!nodeChildren && !nodeChildren->empty())
        {
            for (const auto& [key, node] : *nodeChildren)
            {
                if (node.enteringCondtion())
                {
                    if (activeOnLastUpdate.empty())
                    {
                        node.onEnter();
                    }
                    else if (auto last = activeOnLastUpdate.front(); last != &node)
                    {
                        last->onExit();
                        node.onEnter();
                        activeOnLastUpdate.pop();
                    }
                    node.onUpdate();
                    activeOnCurrentUpdate.push(&node);
                    nodeChildren = &node.children;
                    break;
                }
            }
        }
        activeOnLastUpdate = activeOnCurrentUpdate;
    }
};