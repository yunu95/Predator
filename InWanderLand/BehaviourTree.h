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
    private:
        std::unordered_map<int, Node> children;
        std::vector<Node*> childrenInOrder;
    public:
        std::function<void()> onEnter = []() {};
        std::function<void()> onExit = []() {};
        std::function<void()> onUpdate = []() {};
        std::function<bool()> enteringCondtion = []() {return false; };
        Node& operator[](int key)
        {
            if (!children.contains(key))
            {
                children[key] = Node{};
                childrenInOrder.push_back(&children[key]);
            }
            return children[key];
        }
        friend BehaviourTree;
    };
    std::unordered_map<int, Node> rootChildren;
    std::vector<Node*> rootChildrenInOrder;
    std::queue<const Node*> activeOnLastUpdate;
    std::queue<const Node*> activeOnCurrentUpdate;
    void Update()
    {
        activeOnCurrentUpdate = std::queue<const Node*>{};
        const auto* nodeChildren = &rootChildrenInOrder;
        while (!nodeChildren && !nodeChildren->empty())
        {
            for (const auto& node : *nodeChildren)
            {
                if (node->enteringCondtion())
                {
                    if (activeOnLastUpdate.empty())
                    {
                        node->onEnter();
                    }
                    else if (auto last = activeOnLastUpdate.front(); last != node)
                    {
                        last->onExit();
                        node->onEnter();
                        activeOnLastUpdate.pop();
                    }
                    node->onUpdate();
                    activeOnCurrentUpdate.push(node);
                    nodeChildren = &node->childrenInOrder;
                    break;
                }
            }
        }
        activeOnLastUpdate = activeOnCurrentUpdate;
    }
    Node& operator[](int key)
    {
        if (!rootChildren.contains(key))
        {
            rootChildren[key] = Node{};
            rootChildrenInOrder.push_back(&rootChildren[key]);
        }
        return rootChildren[key];
    }
};