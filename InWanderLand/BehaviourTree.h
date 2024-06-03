#include <vector>
#include <functional>
#include <unordered_set>
#include <unordered_map>

class BehaviourTree
{
public:
    struct Node
    {
    private:
        int nodeKey;
        std::unordered_map<int, Node> children;
        std::vector<Node*> childrenInOrder;
    public:
        int GetNodeKey() const { return nodeKey; };
        std::function<void()> onEnter = []() {};
        std::function<void()> onExit = []() {};
        std::function<void()> onUpdate = []() {};
        std::function<bool()> enteringCondtion = []() {return false; };
        Node& operator[](int key)
        {
            if (!children.contains(key))
            {
                children[key] = Node{};
                children[key].nodeKey = key;
                childrenInOrder.push_back(&children[key]);
            }
            return children[key];
        }
        friend BehaviourTree;
    };
    const std::vector<const Node*>& GetActiveNodes()const { return activeOnCurrentUpdate; };
    void Update()
    {
        activeOnCurrentUpdate.clear();
        const auto* nodeChildren = &rootChildrenInOrder;
        while (nodeChildren && !nodeChildren->empty())
        {
            auto nodeChildrenBefore = nodeChildren;
            for (const auto& node : *nodeChildren)
            {
                if (node->enteringCondtion())
                {
                    activeOnCurrentUpdate.push_back(node);
                    nodeChildren = &node->childrenInOrder;
                    break;
                }
            }
            assert(nodeChildrenBefore != nodeChildren, "behaviour node can't reach the leaf node.");
        }
        // 먼저 Exit를 순차적으로 부르고 다음 Enter들을 부른다.
        for (int i = activeOnLastUpdate.size() - 1; i >= 0; i--)
        {
            if (activeOnCurrentUpdate.size() - 1 < i || activeOnLastUpdate[i] != activeOnCurrentUpdate[i])
            {
                activeOnLastUpdate[i]->onExit();
            }
        }
        for (int i = 0; i < activeOnCurrentUpdate.size(); i++)
        {
            if (((int)activeOnLastUpdate.size() - 1 < i) || (activeOnLastUpdate[i] != activeOnCurrentUpdate[i]))
            {
                activeOnCurrentUpdate[i]->onEnter();
            }
            activeOnCurrentUpdate[i]->onUpdate();
        }
        activeOnLastUpdate = activeOnCurrentUpdate;
    }
    Node& operator[](int key)
    {
        if (!rootChildren.contains(key))
        {
            rootChildren[key] = Node{};
            rootChildren[key].nodeKey = key;
            rootChildrenInOrder.push_back(&rootChildren[key]);
        }
        return rootChildren[key];
    }
private:
    std::vector<const Node*> activeOnLastUpdate;
    std::vector<const Node*> activeOnCurrentUpdate;
    std::unordered_map<int, Node> rootChildren;
    std::vector<Node*> rootChildrenInOrder;
};