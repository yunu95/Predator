#include "YunutyEngine.h"
#include "Scene.h"
#include "GameObject.h"

void ExpandBoundingVolume(yunuGI::Vector3* boundingMin, yunuGI::Vector3* boundingMax, const yunuGI::Vector3& newBoundingMin, const yunuGI::Vector3& newBoundingMax)
{
    if (boundingMin)
    {
        boundingMin->x = min(boundingMin->x, newBoundingMin.x);
        boundingMin->y = min(boundingMin->y, newBoundingMin.y);
        boundingMin->z = min(boundingMin->z, newBoundingMin.z);
    }
    if (boundingMax)
    {
        boundingMax->x = max(boundingMax->x, newBoundingMax.x);
        boundingMax->y = max(boundingMax->y, newBoundingMax.y);
        boundingMax->z = max(boundingMax->z, newBoundingMax.z);
    }
}
void AddGameObjectFromFBXNode(GameObject* parentObject, yunuGI::FBXData* fbxNode, const std::string& fbxName, GameObject* rootObject, yunuGI::Vector3* boundingMin, yunuGI::Vector3* boundingMax, bool boundsInit = true)
{
    auto gameObjectChild = parentObject->AddGameObject();
    gameObjectChild->setName(std::string{ fbxNode->nodeName.begin(), fbxNode->nodeName.end() });

    gameObjectChild->GetTransform()->SetLocalPosition(Vector3d{ fbxNode->pos.x,fbxNode->pos.y ,fbxNode->pos.z });
    gameObjectChild->GetTransform()->SetLocalScale(Vector3d{ fbxNode->scale.x,fbxNode->scale.y ,fbxNode->scale.z });
    gameObjectChild->GetTransform()->SetLocalRotation(Quaternion{ fbxNode->quat.w, fbxNode->quat.x,fbxNode->quat.y,fbxNode->quat.z });

    if (!fbxNode->hasAnimation)
    {
        if (fbxNode->materialVec.size() != 0)
        {
            auto renderer = gameObjectChild->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
            auto mesh = graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(fbxNode->meshName);
            yunuGI::Vector3 meshBoundingMin, meshBoundingMax;
            mesh->GetBoundingBoxInfo(&meshBoundingMin, &meshBoundingMax);
            if (boundingMin && boundingMax)
            {
                if (boundsInit)
                {
                    ExpandBoundingVolume(boundingMin, boundingMax, meshBoundingMin, meshBoundingMax);
                }
                else
                {
                    *boundingMin = meshBoundingMin;
                    *boundingMax = meshBoundingMax;
                }
            }

            renderer->GetGI().SetMesh(mesh);

            // Material Data Set
            for (int j = 0; j < fbxNode->materialVec.size(); ++j)
            {
                auto temp = graphics::Renderer::SingleInstance().GetResourceManager()->GetMaterial(fbxNode->materialVec[j].materialName);
                renderer->GetGI().SetMaterial
                (
                    j, graphics::Renderer::SingleInstance().GetResourceManager()->GetMaterial(fbxNode->materialVec[j].materialName)
                );
            }
        }
    }
    else
    {
        if (fbxNode->materialVec.size() != 0)
        {
            auto renderer = gameObjectChild->AddComponent<yunutyEngine::graphics::SkinnedMesh>();
            auto mesh = graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(fbxNode->meshName);
            renderer->GetGI().SetMesh(mesh);
            renderer->GetGI().SetBone(std::wstring{ fbxName.begin(), fbxName.end() });

            auto animator = rootObject->GetComponent<yunutyEngine::graphics::Animator>();
            auto animatorIndex = animator->GetGI().GetID();
            renderer->GetGI().SetAnimatorIndex(animatorIndex);

            yunuGI::IShader* shader = nullptr;
            auto& shaderList = graphics::Renderer::SingleInstance().GetResourceManager()->GetShaderList();

            for (auto& i : shaderList)
            {
                if (i->GetName() == L"SkinnedVS.cso")
                {
                    shader = i;
                }
            }

            // Material Data Set
            for (int j = 0; j < fbxNode->materialVec.size(); ++j)
            {
                auto material = graphics::Renderer::SingleInstance().GetResourceManager()->GetMaterial(fbxNode->materialVec[j].materialName);
                material->SetVertexShader(shader);
                renderer->GetGI().SetMaterial
                (
                    j, material
                );
            }
        }

    }

    for (int i = 0; i < fbxNode->child.size(); ++i)
    {
        AddGameObjectFromFBXNode(gameObjectChild, fbxNode->child[i], fbxName, rootObject, boundingMin, boundingMax);
    }
}

using namespace yunutyEngine;
yunutyEngine::Scene* yunutyEngine::Scene::currentScene = nullptr;
yunutyEngine::Scene* yunutyEngine::Scene::getCurrentScene()
{
    return currentScene;
}
void yunutyEngine::Scene::LoadScene(Scene* scene)
{
    currentScene = scene;
}
string yunutyEngine::Scene::getName()const
{
    return name;
}
void yunutyEngine::Scene::setName(const string& name)
{
    this->name = name;
}
yunutyEngine::GameObject* yunutyEngine::Scene::AddGameObject(IGameObjectParent* parent)
{
    auto gameObject = new GameObject(parent != nullptr ? parent : this);
    gameObject->scene = this;

    return gameObject;
}

yunutyEngine::GameObject* Scene::AddGameObjectFromFBX(string fbxName, yunuGI::Vector3* boundingMin, yunuGI::Vector3* boundingMax)
{
    auto gameObject = AddGameObject(fbxName);
    yunuGI::FBXData* data = nullptr;

    if (graphics::Renderer::SingleInstance().GetResourceManager()->GetFBXData(fbxName, data))
    {
        if (data->hasAnimation)
        {
            auto animator = gameObject->AddComponent<yunutyEngine::graphics::Animator>();
            animator->GetGI().SetModel(std::wstring{ fbxName.begin(), fbxName.end() });
        }
        bool boundsInit = false;
        for (int i = 0; i < data->child.size(); ++i)
        {
            AddGameObjectFromFBXNode(gameObject, data->child[i], fbxName, gameObject, boundingMin, boundingMax, boundsInit);
            boundsInit = true;
        }
    }

    return gameObject;
}

yunutyEngine::GameObject* yunutyEngine::Scene::AddGameObject(string name, IGameObjectParent* parent)
{
    auto gameObject = AddGameObject(parent);
    gameObject->name = name;
    return gameObject;
}
unique_ptr<yunutyEngine::GameObject> yunutyEngine::Scene::MoveChild(GameObject* child)
{
    unique_ptr<yunutyEngine::GameObject> ret = move(children[child]);
    children.erase(child);
    auto erasedIndex = childIndexMap[child];
    childIndexMap.erase(child);
    childrenIndexed.erase(childrenIndexed.begin() + erasedIndex);
    for (auto& each : childIndexMap)
        if (each.second > erasedIndex)
            each.second--;
    child->parent = nullptr;
    HandleChildUpdateState(child);
    return ret;
}

const vector<yunutyEngine::GameObject*>& yunutyEngine::Scene::GetChildren() const
{
    return childrenIndexed;
}
void yunutyEngine::Scene::ReceiveChild(remove_reference<unique_ptr<GameObject>>::type&& child)
{
    auto ptr = child.get();
    children.insert(make_pair(ptr, unique_ptr<GameObject>()));
    children[ptr] = move(child);
    children.emplace(ptr, move(child));
    childIndexMap.insert(make_pair(ptr, childrenIndexed.size()));
    childrenIndexed.push_back(ptr);
}
int yunutyEngine::Scene::GetChildIndex(const GameObject* child)const
{
    return childIndexMap.find(child)->second;
}
void yunutyEngine::Scene::SetChildIndex(GameObject* child, int index)
{
    if (index >= children.size())
        index = children.size() - 1;
    if (index < 0)
        index = 0;

    if (children.find(child) == children.end())
        return;
    auto origin = childIndexMap[child];
    if (origin == index)
        return;

    if (origin > index)
    {
        for (int i = origin; i > index; i--)
            childrenIndexed[i] = childrenIndexed[i - 1];
        childrenIndexed[index] = child;

        for (int i = origin; i >= index; i--)
            childIndexMap[childrenIndexed[i]] = i;
    }
    else
    {
        for (int i = origin; i < index; i++)
            childrenIndexed[i] = childrenIndexed[i + 1];
        childrenIndexed[index] = child;

        for (int i = origin; i <= index; i++)
            childIndexMap[childrenIndexed[i]] = i;
    }
}
void yunutyEngine::Scene::DestroyGameObject(GameObject* gameObj)
{
    if (!gameObj)
        return;
    destroyList.insert(gameObj);
}
