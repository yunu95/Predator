#include "ResourceManager.h"

#include <assert.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <limits>  
#include <locale>  
#include <sstream> 
#include <thread>
#include <future>

#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "VertexShader.h"
#include "GeometryShader.h"
#include "PixelShader.h"
#include "ResourceBuilder.h"
#include "Device.h"
#include "SwapChain.h"
#include "Mesh.h"
#include "IMaterial.h"
#include "Material.h"
#include "Texture.h"
#include "Animation.h"
#include "AnimationGroup.h"

#include "NailEngine.h"
#include "RenderTargetGroup.h"

#include "ModelLoader.h"
#include "Video.h"
#include "MaterialWrapper.h"




LazyObjects<ResourceManager> ResourceManager::Instance;

void ResourceManager::CreateDefaultResource()
{
    CreateDefaultShader();
    CreateDefaultMesh();
    CreateDefaultTexture();
    CreateDefaultMaterial();
}

void ResourceManager::CreateShader(const std::wstring& shaderPath)
{
    size_t dotPos = shaderPath.find_last_of(L".");

    std::wstring shaderType = shaderPath.substr(dotPos - 2, 2);
    std::shared_ptr<yunuGI::IShader> shader = nullptr;

    std::filesystem::path _shaderName(shaderPath);
    std::wstring shaderName = _shaderName.filename().wstring();

    auto iter = shaderMap.find(shaderName);
    if (iter == shaderMap.end())
    {
        if (shaderType == L"VS")
        {
            shader = std::make_shared<VertexShader>();
        }
        else if (shaderType == L"PS")
        {
            shader = std::make_shared<PixelShader>();
        }
        else if (shaderType == L"GS")
        {
            shader = std::make_shared<GeometryShader>();
        }


        if (shader != nullptr)
        {
            std::static_pointer_cast<Shader>(shader)->CreateShader(shaderPath);
        }

        shader->SetName(shaderName);

        shaderVec.emplace_back(shader.get());
        shaderMap.insert({ shaderName , std::move(shader) });
    }
}

void ResourceManager::CreateMesh(const std::wstring& mesh)
{
    if (mesh == L"Rectangle")
    {
        LoadRactangleMesh();
        return;
    }
    else if (mesh == L"Line")
    {
        LoadLineMesh();
        return;
    }
    else if (mesh == L"Point")
    {
        LoadPointMesh();
        return;
    }
    //else if (mesh == L"Sphere")
    //{
    //	LoadSphereMesh();
    //	return;
    //}
    //else if (mesh == L"Rectangle")
    //{
    //	LoadRactangleMesh();
    //	return;
    //}


    //else if (mesh == L"Capsule")
    //{
    //	LoadCapsuleMesh();
    //	return;
    //}
    //else if (mesh == L"Cylinder")
    //{
    //	LoadCylinderMesh();
    //	return;
    //}
    //else if (mesh == L"Triangle")
    //{
    //	LoadTriangleMesh();
    //	return;
    //}
}

void ResourceManager::CreateMesh(const std::shared_ptr<Mesh>& mesh)
{
    meshVec.emplace_back(mesh.get());
    meshMap.insert({ mesh->GetName(), mesh });
}

void ResourceManager::DeleteMesh(yunuGI::IMesh* mesh)
{
    if (mesh == nullptr)
        return;

    for (auto i = 0; i < meshVec.size(); i++)
    {
        if (meshVec[i] == mesh)
        {
            meshVec.erase(meshVec.begin() + i);
            break;
        }
    }
    meshMap.erase(mesh->GetName());
}

void ResourceManager::DeleteDeferredTexture()
{
    for (auto& each : this->deferredTextureMap)
    {

    }
}

void ResourceManager::DeleteMaterial(yunuGI::IMaterial* mat)
{
}

void ResourceManager::OrganizeResource()
{

}

yunuGI::IMesh* ResourceManager::CreateMesh(std::wstring meshName, std::vector<yunuGI::Vector3>& posVec, std::vector<unsigned int>& idxVec, std::vector<yunuGI::Vector3>& normalVec, const std::vector<yunuGI::Vector2>& uvVec)
{
    std::shared_ptr<Mesh> tempMesh = std::make_shared<Mesh>();

    tempMesh->SetName(meshName);

    std::vector<Vertex> vertices;

    for (int i = 0; i < posVec.size(); ++i)
    {
        DirectX::SimpleMath::Vector3 tempNormal = normalVec.size() == 0 ? DirectX::SimpleMath::Vector3{ 0.f, 0.f, 0.f } : DirectX::SimpleMath::Vector3{ normalVec[i].x,normalVec[i].y ,normalVec[i].z };
        vertices.emplace_back(Vertex{ DirectX::SimpleMath::Vector3{posVec[i].x, posVec[i].y, posVec[i].z},
                          DirectX::SimpleMath::Vector4{1.f,1.f,1.f,1.f},
                          uvVec.empty() ? DirectX::SimpleMath::Vector2{0.5f,0.5f} : DirectX::SimpleMath::Vector2{uvVec[i].x,uvVec[i].y},
                          DirectX::SimpleMath::Vector2{0.5f,0.5f},
                          tempNormal,
                          DirectX::SimpleMath::Vector3{0.0f, 0, -1.f } });
    }


    DirectX::SimpleMath::Vector3 minPoint = vertices[0].pos;
    DirectX::SimpleMath::Vector3 maxPoint = vertices[0].pos;

    for (const auto& vertex : vertices) {
        // minPoint 업데이트
        minPoint.x = std::min(minPoint.x, vertex.pos.x);
        minPoint.y = std::min(minPoint.y, vertex.pos.y);
        minPoint.z = std::min(minPoint.z, vertex.pos.z);

        // maxPoint 업데이트
        maxPoint.x = std::max(maxPoint.x, vertex.pos.x);
        maxPoint.y = std::max(maxPoint.y, vertex.pos.y);
        maxPoint.z = std::max(maxPoint.z, vertex.pos.z);
    }

    tempMesh->SetData(vertices, idxVec, maxPoint, minPoint);
    CreateMesh(tempMesh);

    return tempMesh.get();
}

void* ResourceManager::GetFinalRenderImage()
{
    ID3D11Texture2D* backBuffer = nullptr;
    ID3D11Texture2D* renderImage = nullptr;

    ResourceBuilder::Instance.Get().swapChain->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&backBuffer));

    //backBuffer = NailEngine::Instance.Get().GetRenderTargetGroup()[static_cast<int>(RENDER_TARGET_TYPE::FINAL)]->GetRTTexture(static_cast<int>(FINAL))->GetTex2D().Get();

    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&desc, nullptr, &renderImage);
    ResourceBuilder::Instance.Get().device->GetDeviceContext()->CopyResource(renderImage, backBuffer);

    if (renderImageView != nullptr)
    {
        renderImageView->Release();
        renderImageView = nullptr;
    }
    ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(renderImage, nullptr, &renderImageView);

    backBuffer->Release();
    renderImage->Release();

    return static_cast<void*>(renderImageView);
}

void* ResourceManager::GetDevice()
{
    return ResourceBuilder::Instance.Get().device->GetDevice().Get();
}

void* ResourceManager::GetDeviceContext()
{
    return ResourceBuilder::Instance.Get().device->GetDeviceContext().Get();
}

void ResourceManager::PushFBXBoneInfo(const std::wstring fbxName, std::map<std::wstring, BoneInfo>& boneInfoMap)
{
    this->fbxBoneInfoMap.insert({ fbxName, (boneInfoMap) });
}

void ResourceManager::PushFBXNode(const std::wstring fbxName, FBXNode* fbxNode)
{
    this->fbxNodeMap.insert({ fbxName, fbxNode });
}

yunuGI::IMaterial* ResourceManager::CrateMaterial(std::wstring materialName)
{
    std::shared_ptr<Material> material = std::make_shared<Material>();

    if (materialName.empty())
    {
        materialName = L"DefaultMaterial";
    }

    material->SetName(materialName);

    materialMap.insert({ materialName, material });
    materialVec.push_back(material.get());

    return material.get();
}

Material* ResourceManager::CreateInstanceMaterial(const Material* material)
{
    Material* _material = new Material(*material);

    std::wstring materialName = _material->GetName();
    materialName += L"_instance_";
    materialName += std::to_wstring(_material->GetID());
    _material->SetName(materialName);

    instanceMaterialMap.insert({ materialName, std::shared_ptr<Material>(_material) });

    return _material;
}

yunuGI::IMaterial* ResourceManager::CloneMaterial(std::wstring materialName, yunuGI::IMaterial* material)
{
    material = (material)->GetMaterial();
    if (this->materialMap.find(materialName) != this->materialMap.end())
    {
        return this->materialMap[materialName].get();
    }
    else
    {
        Material* _material = new Material(*(static_cast<Material*>(material)));

        _material->SetName(materialName);

        this->materialMap.insert({ materialName, std::shared_ptr<Material>(_material) });
        this->materialVec.emplace_back(_material);
        return _material;
    }
}

void ResourceManager::CreateTexture(const std::wstring& texturePath)
{
    auto iter = textureMap.find(texturePath);
    if (iter == textureMap.end())
    {
        std::shared_ptr<Texture> texture = std::make_shared<Texture>();

        texture->LoadTexture(texturePath);
        texture->SetName(texturePath);

        textureMap.insert({ texturePath, texture });
        textureVec.push_back(texture.get());
    }
}
// 비동기적으로 텍스처를 생성
void ResourceManager::CreateTextures(const std::vector<std::wstring>& texturePaths)
{
    static constexpr int textureCountPerThread = 10000;
    std::vector<std::future<std::vector<std::shared_ptr<Texture>>>> textures;
    for (int i = 0; i < texturePaths.size(); i += textureCountPerThread)
    {
        std::vector<std::wstring> tempTexturePaths;
        for (int j = 0; j < textureCountPerThread; ++j)
        {
            if (i + j >= texturePaths.size())
                break;
            tempTexturePaths.push_back(texturePaths[i + j]);
        }
        textures.push_back(std::async(std::launch::async, [tempTexturePaths]() {
            std::vector<std::shared_ptr<Texture>> textureChunk;
            for (int i = 0; i < tempTexturePaths.size(); ++i)
            {
                std::shared_ptr<Texture> texture = std::make_shared<Texture>();
                texture->LoadTexture(tempTexturePaths[i]);
                texture->SetName(tempTexturePaths[i]);
                textureChunk.push_back(texture);
            }
            return textureChunk;
            }));
    }
    /*for (auto& each : texturePaths)
    {
        auto iter = textureMap.find(each);
        if (iter == textureMap.end())
        {
            textures.push_back(std::async(std::launch::async, [each]() {
                try
                {

                    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
                    texture->LoadTexture(each);
                    texture->SetName(each);
                    return texture;
                }
                catch (const std::exception& e)
                {
                    return std::shared_ptr<Texture>();
                }
                }));
        }
    }*/
    for (auto& each : textures)
    {
        auto eachChunk = each.get();
        for (auto& texture : eachChunk)
        {
            textureMap.insert({ texture->GetName(), texture });
            textureVec.push_back(texture.get());
        }
    }
}

std::shared_ptr<yunuGI::ITexture>& ResourceManager::CreateTexture(const std::wstring& texturePath, unsigned int width, unsigned int height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlag, bool isMSAA, int arraySize, int sliceCount)
{
    auto iter = this->deferredTextureMap.find(texturePath);
    if (iter == this->deferredTextureMap.end())
    {
        std::shared_ptr<Texture> texture = std::make_shared<Texture>();

        texture->CreateTexture(texturePath, width, height, format, bindFlag, isMSAA, arraySize, sliceCount);
        texture->SetName(texturePath);

        this->deferredTextureMap.insert({ texturePath, texture });
        //this->textureMap.insert({ texturePath, texture });

        return this->deferredTextureMap[texturePath];
    }
    else
    {
        std::static_pointer_cast<Texture>(iter->second)->Release();
        std::static_pointer_cast<Texture>(iter->second)->CreateTexture(texturePath, width, height, format, bindFlag, arraySize, sliceCount);

        return iter->second;
    }
}

std::shared_ptr<Texture>& ResourceManager::CreateTextureFromResource(const std::wstring& texturePath, Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D)
{
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();

    texture->CreateFromResource(tex2D);
    texture->SetName(texturePath);

    textureMap.insert({ texturePath, texture });

    return texture;
}

void ResourceManager::LoadVFXFrameInfo(const std::wstring& vfxPath)
{
    std::ifstream file(vfxPath);

    if (!file.is_open())
    {
        return;
    }


    nlohmann::json jsonData;
    file >> jsonData;

    file.close();

    for (const auto& frameJson : jsonData)
    {
        std::string MaterialName = frameJson.at("material_name").get<std::string>();;
        std::wstring wMaterialName = this->String_To_Wstring(MaterialName);
        float frameRate = frameJson.at("frame_rate").get<float>();;

        std::vector<yunuGI::VFXInfo> locationVec;
        float frame;
        for (const auto& loc : frameJson.at("location_data"))
        {
            yunuGI::VFXInfo info;
            info.frame = loc.at("frame").get<int>();
            const auto& locationJson = loc.at("location");
            info.location.x = locationJson[0].get<float>();
            info.location.y = locationJson[1].get<float>();
            locationVec.push_back(info);
        }

        // 비어있는 프레임을 채우는 부분
        //std::vector<yunuGI::VFXInfo> frameVec;
        //frameVec.resize(locationVec.back().frame + 1);
        //for (int i = 0; i < locationVec.size() - 1; ++i)
        //{
        //    int curFrameCount = locationVec[i].frame;
        //    int nextFrameCount = locationVec[i + 1].frame;
        //    int totalFrame = nextFrameCount - curFrameCount;

        //    if (nextFrameCount == locationVec.back().frame)
        //    {
        //        nextFrameCount++;
        //    }
        //    int count = 0;
        //    for (int j = curFrameCount; j < nextFrameCount; ++j)
        //    {
        //        float ratio = ((float)count / totalFrame);

        //        auto lerpLocation = yunuGI::Vector2::Lerp(locationVec[i].location, locationVec[i+1].location, ratio);
        //        frameVec[j].frame = j;
        //        frameVec[j].location = lerpLocation;
        //        count++;
        //    }
        //}

        this->vfxFrameInfoMap.insert({ wMaterialName,{frameRate, locationVec} });
        locationVec.clear();
    }
}

void ResourceManager::LoadFBX(const char* filePath)
{
    std::wstring fbxName = std::filesystem::path(filePath).stem().wstring();

    FBXNode* node = ModelLoader::Instance.Get().LoadModel(filePath);
    //////////////////////////////////////////////////////////////////////////this->fbxNodeMap.insert({ fbxName, node });

    auto iter = fbxDataMap.find(fbxName);

    if (iter == fbxDataMap.end())
    {
        yunuGI::FBXData* fbxData = new yunuGI::FBXData;
        fbxData->hasAnimation = node->hasAnimation;
        fbxData->nodeName = node->nodeName;
        fbxData->child.resize(node->child.size());

        for (int i = 0; i < node->child.size(); ++i)
        {
            fbxData->child[i] = new yunuGI::FBXData;
            this->FillFBXData(fbxName, node->child[i], fbxData->child[i], false);
        }

        this->fbxDataMap.insert({ fbxName, fbxData });
        this->fbxVec.emplace_back(fbxName);
    }
    else
    {
        for (int i = 0; i < node->child.size(); ++i)
        {
            this->FillFBXData(fbxName, node->child[i], iter->second->child[i], true);
        }
    }
}
void ResourceManager::LoadVideo(const std::wstring& videoPath)
{
    //std::wstring videoPath = std::filesystem::path(filePath).wstring();
    auto iter = videoDataMap.find(videoPath);
    if (iter == videoDataMap.end())
    {
        std::shared_ptr<Video> video = std::make_shared<Video>();
        video->open(videoPath.c_str());
        video->SetName(videoPath);
        videoDataMap.insert({ videoPath, video });
    }
}

void ResourceManager::CreateAnimation(const std::vector<AnimationClip>& animationClip, const std::wstring& fbxName)
{
    for (int i = 0; i < animationClip.size(); ++i)
    {
        auto iter = this->animationMap.find(animationClip[i].name);

        if (iter == this->animationMap.end())
        {
            std::shared_ptr<yunuGI::IAnimation> animation = std::make_shared<Animation>();

            animation->SetName(animationClip[i].name);
            animation->SetDuration(animationClip[i].duration);
            animation->SetTotalFrame(animationClip[i].totalFrame);

            std::static_pointer_cast<Animation>(animation)->SetAnimationClip((animationClip[i]));

            this->animationVec.emplace_back(animation.get());
            this->animationMap.insert({ animation->GetName(), animation });

            auto iter = this->animationGroupMap.find(fbxName);
            if (iter != this->animationGroupMap.end())
            {
                std::static_pointer_cast<Animation>(animation)->SetAnimationIndex(this->animationGroupMap[fbxName]->GetAnimationVec().size());
                this->animationGroupMap[fbxName]->GetAnimationVec().emplace_back(std::static_pointer_cast<Animation>(animation));
            }
            else
            {
                std::shared_ptr<AnimationGroup> animationGroup = std::make_shared<AnimationGroup>();
                animationGroup->SetFBXName(fbxName);

                std::static_pointer_cast<Animation>(animation)->SetAnimationIndex(animationGroup->GetAnimationVec().size());
                animationGroup->GetAnimationVec().emplace_back(std::static_pointer_cast<Animation>(animation));
                this->animationGroupMap.insert({ fbxName, animationGroup });
            }


        }
    }

    this->animationGroupMap[fbxName]->CreateTexture();
    std::vector<yunuGI::IAnimation*> tempAnimationVec;
    for (int i = 0; i < this->animationGroupMap[fbxName]->GetAnimationVec().size(); ++i)
    {
        tempAnimationVec.push_back(this->animationGroupMap[fbxName]->GetAnimationVec()[i].get());
    }
    this->fbxAnimationListMap.insert({ fbxName , tempAnimationVec });
}

std::shared_ptr<yunuGI::IMaterial> ResourceManager::GetMaterial(const std::wstring& materialName)
{
    auto iter = materialMap.find(materialName);
    if (iter == materialMap.end())
    {
        auto iter2 = instanceMaterialMap.find(materialName);
        if (iter2 == instanceMaterialMap.end())
        {
            return nullptr;
        }
        else
        {
            return iter2->second;
        }
    }
    else
    {
        return iter->second;
    }
}

std::shared_ptr<yunuGI::IMaterial> ResourceManager::GetInstanceMaterial(const std::wstring& materialName)
{
    auto iter2 = instanceMaterialMap.find(materialName);
    if (iter2 == instanceMaterialMap.end())
    {
        return nullptr;
    }
    else
    {
        return iter2->second;
    }
}

std::shared_ptr<yunuGI::IShader> ResourceManager::GetShader(const std::wstring& shaderPath)
{
    auto iter = shaderMap.find(shaderPath);
    if (iter == shaderMap.end())
    {
        return std::shared_ptr<yunuGI::IShader>{};
    }
    //assert(iter != shaderMap.end());

    return std::static_pointer_cast<Shader>(iter->second);
}

std::shared_ptr<yunuGI::IShader> ResourceManager::GetShader(const yunuGI::IShader* shader)
{
    auto iter = this->shaderMap.find(shader->GetName());
    if (iter != this->shaderMap.end())
    {
        return iter->second;
    }

    iter = this->deferredShaderMap.find(shader->GetName());
    if (iter != this->deferredShaderMap.end())
    {
        return iter->second;
    }

    return nullptr;
}

std::shared_ptr<yunuGI::IShader> ResourceManager::GetDeferredShader(const std::wstring& shaderPath)
{
    auto iter = deferredShaderMap.find(shaderPath);
    assert(iter != deferredShaderMap.end());

    return std::static_pointer_cast<Shader>(iter->second);
}

std::shared_ptr<Mesh> ResourceManager::GetMesh(const std::wstring& meshName)
{
    if (meshName == L"Rectangle")
    {
        return rectangleMesh;
    }

    auto iter = meshMap.find(meshName);
    assert(iter != meshMap.end());

    return std::static_pointer_cast<Mesh>(iter->second);
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const std::wstring& textureName)
{
    auto iter = textureMap.find(textureName);
    if (iter != textureMap.end())
    {
        return std::static_pointer_cast<Texture>(iter->second);
    }
    iter = deferredTextureMap.find(textureName);
    if (iter != deferredTextureMap.end())
    {
        return std::static_pointer_cast<Texture>(iter->second);
    }
    return nullptr;
}

yunuGI::FBXData* ResourceManager::GetFBXData(const std::string fbxName)
{
    std::wstring fbxNameW = std::wstring{ fbxName.begin(), fbxName.end() };

    auto iter = this->fbxDataMap.find(fbxNameW);
    assert(iter != this->fbxDataMap.end());
    return iter->second;
}
std::weak_ptr<yunuGI::IVideo> ResourceManager::GetVideoData(const std::wstring& videoPath)const
{
    auto iter = this->videoDataMap.find(videoPath);
    if (iter != this->videoDataMap.end())
    {
        return iter->second;
    }
    return std::weak_ptr<yunuGI::IVideo>();
}

//std::vector<FBXBoneInfo>& ResourceManager::GetFBXBoneData(const std::string fbxName)
//{
//	std::wstring fbxNameW = std::wstring{ fbxName.begin(), fbxName.end() };
//
//	auto iter = this->BoneOffsetInfoVecMap.find(fbxNameW);
//	assert(iter != this->BoneOffsetInfoVecMap.end());
//
//	return iter->second;
//}

yunuGI::BoneInfo& ResourceManager::GetBoneData(const std::string fbxName)
{
    yunuGI::BoneInfo boneInfo;
    return boneInfo;
}

std::map<std::wstring, BoneInfo>& ResourceManager::GetFBXBoneData(const std::string fbxName)
{
    auto iter = this->fbxBoneInfoMap.find(std::wstring{ fbxName.begin(), fbxName.end() });
    if (iter != this->fbxBoneInfoMap.end())
    {
        return iter->second;
    }
}

FBXNode* ResourceManager::GetFBXNode(const std::wstring& fbxName)
{
    return this->fbxNodeMap.find(fbxName)->second;
}

std::pair<float, std::vector<yunuGI::VFXInfo>>& ResourceManager::GetVFXInfo(const std::wstring& materialName)
{
    size_t pos = materialName.find(L"_instance");
    if (pos != std::wstring::npos)
    {
        std::wstring result = materialName.substr(0, pos);
        return this->vfxFrameInfoMap[result];
    }

    return this->vfxFrameInfoMap[materialName];
}

std::vector<yunuGI::IMesh*>& ResourceManager::GetMeshList() { return this->meshVec; };
std::vector<yunuGI::ITexture*>& ResourceManager::GetTextureList()
{
    return this->textureVec;
};
std::vector<yunuGI::IMaterial*>& ResourceManager::GetMaterialList()
{
    return this->materialVec;
};
std::vector<yunuGI::IShader*>& ResourceManager::GetShaderList() { return this->shaderVec; };
std::vector<yunuGI::IAnimation*>& ResourceManager::GetAnimationList() { return this->animationVec; };

std::vector<std::wstring>& ResourceManager::GetFBXList()
{
    return this->fbxVec;
}

std::shared_ptr<AnimationGroup> ResourceManager::GetAnimationGroup(const std::wstring& modelName)
{
    return this->animationGroupMap.find(modelName)->second;
}

std::vector<yunuGI::IAnimation*>& ResourceManager::GetFBXAnimationList(std::wstring fbxName)
{
    auto iter = this->fbxAnimationListMap.find(fbxName);
    if (iter != this->fbxAnimationListMap.end())
    {
        return this->fbxAnimationListMap[fbxName];
    }

    static std::vector<yunuGI::IAnimation*> tempVec;
    return tempVec;
}

void ResourceManager::SaveFBXData(std::filesystem::path path)
{
    std::ofstream file(path);

    //if (file.is_open())
    //{
    //	for (auto& each : this->fbxDataMap)
    //	{
    //		file << L"FBXName : " << each.first << std::endl;
    //		{
    //			file << L"{" << std::endl;
    //			{
    //				SaveFBXChildData(each.second, file);

    //				for (auto& child : each.second->child)
    //				{
    //					SaveFBXChildData(child, file);
    //				}
    //			}
    //			file << L"}" << std::endl;
    //		}
    //		file << std::endl;
    //	}
    //}

    nlohmann::json jsonArrData;

    for (auto& each : this->fbxDataMap)
    {
        nlohmann::json jsonData;
        std::string fbxName{ each.first.begin(), each.first.end() };
        jsonData["FBXName : "] = fbxName;

        std::string fbxKind = fbxName.substr(0, 3);
        if (fbxKind == "SKM")
        {
            continue;
        }

        SaveFBXChildData(each.second, jsonData);

        jsonArrData.push_back(jsonData);
    }

    if (file.is_open())
    {
        file << std::setw(4) << jsonArrData << std::endl;
    }

    file.close();
}

void ResourceManager::SaveFBXChildData(const yunuGI::FBXData* data, nlohmann::json& jsonData)
{
    jsonData["nodeName"] = std::string{ data->nodeName.begin(), data->nodeName.end() };
    jsonData["meshName"] = std::string{ data->meshName.begin(),data->meshName.end() };

    jsonData["pos"]["x"] = data->pos.x;
    jsonData["pos"]["y"] = data->pos.y;
    jsonData["pos"]["z"] = data->pos.z;

    jsonData["scale"]["x"] = data->scale.x;
    jsonData["scale"]["y"] = data->scale.y;
    jsonData["scale"]["z"] = data->scale.z;

    DirectX::SimpleMath::Vector4 tempQuat = DirectX::XMQuaternionNormalize(DirectX::SimpleMath::Vector4{ data->quat.x, data->quat.y,data->quat.z,data->quat.w });

    jsonData["quat"]["x"] = tempQuat.x;
    jsonData["quat"]["y"] = tempQuat.y;
    jsonData["quat"]["z"] = tempQuat.z;
    jsonData["quat"]["w"] = tempQuat.w;

    jsonData["boneInfo"]["index"] = data->boneInfo.index;

    jsonData["hasAnimation"] = data->hasAnimation;

    jsonData["aabb"]["maxPoint"]["x"] = data->aabb.maxPoint.x;
    jsonData["aabb"]["maxPoint"]["y"] = data->aabb.maxPoint.y;
    jsonData["aabb"]["maxPoint"]["z"] = data->aabb.maxPoint.z;

    jsonData["aabb"]["minPoint"]["x"] = data->aabb.minPoint.x;
    jsonData["aabb"]["minPoint"]["y"] = data->aabb.minPoint.y;
    jsonData["aabb"]["minPoint"]["z"] = data->aabb.minPoint.z;

    for (const auto& material : data->materialVec)
    {
        nlohmann::json materialJson;
        materialJson["materialName"] = std::string{ material.materialName.begin(),material.materialName.end() };
        materialJson["albedoMap"] = std::string{ material.albedoMap.begin(), material.albedoMap.end() };
        materialJson["normalMap"] = std::string{ material.normalMap.begin(), material.normalMap.end() };
        materialJson["armMap"] = std::string{ material.armMap.begin(), material.armMap.end() };
        materialJson["emissionMap"] = std::string{ material.emissionMap.begin(), material.emissionMap.end() };
        materialJson["opacityMap"] = std::string{ material.opacityMap.begin(), material.opacityMap.end() };
        materialJson["heightMap"] = std::string{ material.heightMap.begin(), material.heightMap.end() };
        materialJson["temp0Map"] = std::string{ material.temp0Map.begin(),material.temp0Map.end() };
        materialJson["temp1Map"] = std::string{ material.temp1Map.begin(),material.temp1Map.end() };
        materialJson["temp2Map"] = std::string{ material.temp2Map.begin(),material.temp2Map.end() };
        materialJson["temp3Map"] = std::string{ material.temp3Map.begin(), material.temp3Map.end() };
        materialJson["temp4Map"] = std::string{ material.temp4Map.begin(), material.temp4Map.end() };
        materialJson["temp5Map"] = std::string{ material.temp5Map.begin(), material.temp5Map.end() };
        materialJson["temp6Map"] = std::string{ material.temp6Map.begin(), material.temp6Map.end() };
        materialJson["temp7Map"] = std::string{ material.temp7Map.begin(), material.temp7Map.end() };
        materialJson["temp8Map"] = std::string{ material.temp8Map.begin(), material.temp8Map.end() };
        materialJson["temp9Map"] = std::string{ material.temp9Map.begin(), material.temp9Map.end() };
        materialJson["temp10Map"] = std::string{ material.temp10Map.begin(), material.temp10Map.end() };
        materialJson["temp11Map"] = std::string{ material.temp11Map.begin(), material.temp11Map.end() };
        materialJson["vs"] = std::string{ material.vs.begin(),material.vs.end() };
        auto ps = std::string{ material.ps.begin(), material.ps.end() };
        materialJson["ps"] = ps;

        jsonData["materialVec"].push_back(materialJson);
    }

    jsonData["DiffuseExposure"] = data->diffuseExposure;
    jsonData["AmbientExposure"] = data->ambientExposure;
    jsonData["CastDecal"] = data->castDecal;
    jsonData["CalculateShadow"] = data->calculateShadow;

    for (const auto& child : data->child)
    {
        nlohmann::json childJson;
        SaveFBXChildData(child, childJson);
        jsonData["child"].push_back(childJson);
    }

    //file << L"{" << std::endl;
    //file << L"NodeName : " << data->nodeName << std::endl;
    //file << L"MeshName : " << data->meshName << std::endl;
    //file << L"Position : " << L"x:" << data->pos.x << L", " << L"y:" << data->pos.y << L", " << L"z:" << data->pos.z << std::endl;
    //file << L"Scale : " << L"x:" << data->scale.x << L", " << L"y:" << data->scale.y << L", " << L"z:" << data->scale.z << std::endl;
    //file << L"Quat : " << L"x:" << data->quat.x << L", " << L"y:" << data->quat.y << L", " << L"z:" << data->quat.z << L", " << L"w:" << data->quat.w << std::endl;
    //file << L"BoneInfo : " << data->boneInfo.index << std::endl;
    //file << L"HasAnimation : " << data->hasAnimation << std::endl;
    //file << L"BoundingBox : " << L"x:" << data->aabb.maxPoint.x << L", " << L"y:" << data->aabb.maxPoint.y << L", " << L"z:" << data->aabb.maxPoint.z << L", "
    //	<< L"x:" << data->aabb.minPoint.x << L", " << L"y:" << data->aabb.minPoint.y << L", " << L"z:" << data->aabb.minPoint.z << std::endl;

    //file << L"MaterialCount : " << data->materialVec.size() << std::endl;

    //for (auto& material : data->materialVec)
    //{
    //	file << L"MaterialName : " << material.materialName << std::endl;
    //	file << L"AlbedoMap : " << material.albedoMap << std::endl;
    //	file << L"NormalMap : " << material.normalMap << std::endl;
    //	file << L"ARMMap : " << material.armMap << std::endl;
    //	file << L"EmissionMap : " << material.emissionMap << std::endl;
    //	file << L"OpacityMap : " << material.opacityMap << std::endl;
    //	file << L"HeightMap : " << material.heightMap << std::endl;
    //	file << L"Temp0Map : " << material.temp0Map << std::endl;
    //	file << L"Temp1Map : " << material.temp1Map << std::endl;
    //	file << L"Temp2Map : " << material.temp2Map << std::endl;
    //	file << L"Temp3Map : " << material.temp3Map << std::endl;
    //	file << L"VS : " << material.vs << std::endl;
    //	file << L"PS : " << material.ps << std::endl;
    //}

    //file << L"ChildCount : " << data->child.size() << std::endl;

    //file << L"}" << std::endl;
}

void ResourceManager::LoadFBXData(std::filesystem::path path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        return;
    }


    nlohmann::json jsonData;
    file >> jsonData;

    file.close();

    for (const auto& fbxJson : jsonData)
    {
        auto str = String_To_Wstring(fbxJson["FBXName : "]);
        auto fbxKind = str.substr(0, 3);
        if (fbxKind == L"SKM")
        {
            continue;
        }

        yunuGI::FBXData* fbxData = new yunuGI::FBXData;

        LoadFBXData(fbxJson, fbxData);


        auto iter = fbxDataMap.find(String_To_Wstring(fbxJson["FBXName : "]));
        if (iter == fbxDataMap.end())
        {
            fbxVec.emplace_back(String_To_Wstring(fbxJson["FBXName : "]));
        }
        fbxDataMap[String_To_Wstring(fbxJson["FBXName : "])] = fbxData;
    }
}

void ResourceManager::LoadFBXData(const nlohmann::json& jsonData, yunuGI::FBXData* data)
{
    data->nodeName = String_To_Wstring(jsonData["nodeName"]);
    data->meshName = String_To_Wstring(jsonData["meshName"]);

    data->pos.x = jsonData["pos"]["x"];
    data->pos.y = jsonData["pos"]["y"];
    data->pos.z = jsonData["pos"]["z"];

    data->scale.x = jsonData["scale"]["x"];
    data->scale.y = jsonData["scale"]["y"];
    data->scale.z = jsonData["scale"]["z"];

    data->quat.x = jsonData["quat"]["x"];
    data->quat.y = jsonData["quat"]["y"];
    data->quat.z = jsonData["quat"]["z"];
    data->quat.w = jsonData["quat"]["w"];

    data->boneInfo.index = jsonData["boneInfo"]["index"];

    data->hasAnimation = jsonData["hasAnimation"];

    data->aabb.maxPoint.x = jsonData["aabb"]["maxPoint"]["x"];
    data->aabb.maxPoint.y = jsonData["aabb"]["maxPoint"]["y"];
    data->aabb.maxPoint.z = jsonData["aabb"]["maxPoint"]["z"];

    data->aabb.minPoint.x = jsonData["aabb"]["minPoint"]["x"];
    data->aabb.minPoint.y = jsonData["aabb"]["minPoint"]["y"];
    data->aabb.minPoint.z = jsonData["aabb"]["minPoint"]["z"];

    if (jsonData.contains("materialVec"))
    {
        for (const auto& materialJson : jsonData["materialVec"])
        {
            yunuGI::MaterialData material;
            material.materialName = String_To_Wstring(materialJson["materialName"]);
            material.albedoMap = String_To_Wstring(materialJson["albedoMap"]);
            material.normalMap = String_To_Wstring(materialJson["normalMap"]);
            material.armMap = String_To_Wstring(materialJson["armMap"]);
            material.emissionMap = String_To_Wstring(materialJson["emissionMap"]);
            material.opacityMap = String_To_Wstring(materialJson["opacityMap"]);
            material.heightMap = String_To_Wstring(materialJson["heightMap"]);
            material.temp0Map = String_To_Wstring(materialJson["temp0Map"]);
            material.temp1Map = String_To_Wstring(materialJson["temp1Map"]);
            material.temp2Map = String_To_Wstring(materialJson["temp2Map"]);
            material.temp3Map = String_To_Wstring(materialJson["temp3Map"]);
            material.temp4Map = String_To_Wstring(materialJson["temp4Map"]);
            material.temp5Map = String_To_Wstring(materialJson["temp5Map"]);
            material.temp6Map = String_To_Wstring(materialJson["temp6Map"]);
            material.temp7Map = String_To_Wstring(materialJson["temp7Map"]);
            material.temp8Map = String_To_Wstring(materialJson["temp8Map"]);
            material.temp9Map = String_To_Wstring(materialJson["temp9Map"]);
            material.temp10Map = String_To_Wstring(materialJson["temp10Map"]);
            material.temp11Map = String_To_Wstring(materialJson["temp11Map"]);
            material.vs = String_To_Wstring(materialJson["vs"]);
            material.ps = String_To_Wstring(materialJson["ps"]);

            data->materialVec.push_back(material);
        }
    }

    data->diffuseExposure = jsonData["DiffuseExposure"];
    data->ambientExposure = jsonData["AmbientExposure"];
    data->castDecal = jsonData["CastDecal"];
    data->calculateShadow = jsonData["CalculateShadow"];

    if (jsonData.contains("child"))
    {
        for (const auto& childJson : jsonData["child"])
        {
            yunuGI::FBXData* childData = new yunuGI::FBXData;
            LoadFBXData(childJson, childData);
            data->child.push_back(childData);
        }
    }
}

void ResourceManager::CreateDeferredShader(const std::wstring& shaderPath)
{
    size_t dotPos = shaderPath.find_last_of(L".");

    std::wstring shaderType = shaderPath.substr(dotPos - 2, 2);
    std::shared_ptr<yunuGI::IShader> shader = nullptr;

    if (shaderType == L"VS")
    {
        shader = std::make_shared<VertexShader>();
    }
    else if (shaderType == L"PS")
    {
        shader = std::make_shared<PixelShader>();
    }

    if (shader != nullptr)
    {
        std::static_pointer_cast<Shader>(shader)->CreateShader(shaderPath);
    }

    std::filesystem::path _shaderName(shaderPath);
    std::wstring shaderName = _shaderName.filename().wstring();

    shader->SetName(shaderName);

    deferredShaderMap.insert({ shaderName , std::move(shader) });
}

void ResourceManager::CreateDefaultShader()
{
#pragma region VS
    CreateShader(L"DefaultVS.cso");
    CreateShader(L"DebugVS.cso");
    CreateShader(L"SkinnedVS.cso");
    CreateDeferredShader(L"Deferred_DirectionalLightVS.cso");
    CreateDeferredShader(L"Deferred_PointLightVS.cso");
    CreateDeferredShader(L"Deferred_FinalVS.cso");
    CreateShader(L"TextureVS.cso");
    CreateShader(L"TestVS.cso");
    CreateShader(L"SkinnedShadowVS.cso");
    CreateShader(L"SkyBoxVS.cso");
    CreateShader(L"IrradianceVS.cso");
    CreateShader(L"PreFilteredVS.cso");
    CreateShader(L"SpecLUTVS.cso");
    CreateShader(L"PointLightShadowVS.cso");
    CreateShader(L"Skinned_PointLightShadowVS.cso");
    CreateShader(L"ParticleVS.cso");
    CreateShader(L"TextureAnimVS.cso");
    CreateShader(L"SkinnedVFX_VS.cso");
    CreateShader(L"TestDecalVS.cso");
    CreateShader(L"DissolveVS.cso");
#pragma endregion

#pragma region PS
    CreateShader(L"DefaultPS.cso");
    CreateShader(L"SkinnedPS.cso");
    CreateShader(L"Default_CullFrontPS.cso");
    CreateShader(L"DebugPS.cso");
    CreateShader(L"Debug_AlphaPS.cso");
    CreateDeferredShader(L"Deferred_DirectionalLightPS.cso");
    CreateDeferredShader(L"Deferred_PointLightPS.cso");
    CreateDeferredShader(L"Deferred_FinalPS.cso");
    CreateDeferredShader(L"BackBufferPS.cso");
    CreateShader(L"TexturePS.cso");
    CreateShader(L"TestPS.cso");
    CreateShader(L"SkyBoxPS.cso");
    CreateShader(L"IrradiancePS.cso");
    CreateShader(L"PreFilteredPS.cso");
    CreateShader(L"SpecLUTPS.cso");
    CreateDeferredShader(L"CopyPS.cso");
    CreateDeferredShader(L"BlurPS.cso");
    CreateShader(L"PointLightShadowPS.cso");
    CreateShader(L"ParticlePS.cso");
    CreateShader(L"DissolvePS.cso");
    CreateShader(L"UIPreProcessVS.cso");
    CreateShader(L"UIPreProcessPS.cso");
    CreateShader(L"TextureAnimPS.cso");
    CreateShader(L"RimForwardPS.cso");
    CreateShader(L"GuideLinePS.cso");
    CreateShader(L"Stage1FloorNoBlendPS.cso");
    CreateShader(L"VFX_PS.cso");
    CreateShader(L"MoveAnimTexturePS.cso");
    CreateShader(L"TestDecalPS.cso");
    CreateShader(L"TestDecalMaskPS.cso");
    CreateShader(L"AAPassPS.cso");
    CreateShader(L"Default_AlphaPS.cso");
    CreateShader(L"SilhouettePS.cso");
    CreateShader(L"DissolveShadowPS.cso");
	CreateShader(L"Default_CullFront_AlphaPS.cso");
	CreateShader(L"Window_AlphaPS.cso");
	CreateShader(L"BlankPS.cso");
#pragma endregion

#pragma region GS
    CreateShader(L"PointLightShadowGS.cso");
    CreateShader(L"ParticleGS.cso");
#pragma endregion
}

void ResourceManager::CreateDefaultMesh()
{
    LoadFBX("FBX/Cube/Cube.fbx");
    LoadFBX("FBX/Sphere/Sphere.fbx");
    LoadFBX("FBX/Capsule/Capsule.fbx");
    LoadFBX("FBX/Cylinder/Cylinder.fbx");
    ///CreateMesh(L"Cube");
    ///CreateMesh(L"Sphere");
    CreateMesh(L"Rectangle");
    CreateMesh(L"Point");
    CreateMesh(L"Line");
    ///CreateMesh(L"Capsule");
    ///CreateMesh(L"Cylinder");
    ///CreateMesh(L"Triangle");
}

void ResourceManager::CreateDefaultMaterial()
{
    // DefaultMaterial
    {
        std::wstring name{};
        CrateMaterial(name);
    }

    // DissolveMaterial
    {
        std::wstring name{ L"SkinnedDissolveMaterial" };
        auto material = CrateMaterial(name);
        material->SetTexture(yunuGI::Texture_Type::Temp0, this->GetTexture(L"Texture/Dissolve.jpg").get());
        material->SetVertexShader(this->GetShader(L"SkinnedVS.cso").get());
        material->SetPixelShader(this->GetShader(L"DissolvePS.cso").get());
    }

    // Skinned Default Material
    {
        std::wstring name{ L"SkinnedDefaultMaterial" };
        auto material = CrateMaterial(name);
        material->SetVertexShader(this->GetShader(L"SkinnedVS.cso").get());
        material->SetPixelShader(this->GetShader(L"SkinnedPS.cso").get());
    }

    auto& renderTargetGroupVec = NailEngine::Instance.Get().GetRenderTargetGroup();

    // DirectionalLight
    {
        yunuGI::IMaterial* material = CrateMaterial(L"Deferred_DirectionalLight");
        material->SetVertexShader(GetDeferredShader(L"Deferred_DirectionalLightVS.cso").get());
        material->SetPixelShader(GetDeferredShader(L"Deferred_DirectionalLightPS.cso").get());

        material->SetTexture(yunuGI::Texture_Type::ALBEDO,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(ALBEDO)).get());

        material->SetTexture(yunuGI::Texture_Type::ARM,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(ARM)).get());

        material->SetTexture(yunuGI::Texture_Type::Temp0,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(POSITION)).get());

        material->SetTexture(yunuGI::Texture_Type::Temp1,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(NORMAL)).get());

        material->SetTexture(yunuGI::Texture_Type::Temp2,
            GetTexture(L"ShadowDepth").get());

        material->SetTexture(yunuGI::Texture_Type::Temp3,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(UTIL)).get());
    }

    // PointLight
    {
        yunuGI::IMaterial* material = CrateMaterial(L"Deferred_PointLight");
        material->SetVertexShader(GetDeferredShader(L"Deferred_PointLightVS.cso").get());
        material->SetPixelShader(GetDeferredShader(L"Deferred_PointLightPS.cso").get());
        material->SetTexture(yunuGI::Texture_Type::ALBEDO,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(ALBEDO)).get());
        material->SetTexture(yunuGI::Texture_Type::ARM,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(ARM)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp0,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(POSITION)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp1,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(NORMAL)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp2,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(UTIL)).get());
    }

    // Deferred_Final
    {
        yunuGI::IMaterial* material = CrateMaterial(L"Deferred_Final");
        material->SetVertexShader(GetDeferredShader(L"Deferred_FinalVS.cso").get());
        material->SetPixelShader(GetDeferredShader(L"Deferred_FinalPS.cso").get());
        material->SetTexture(yunuGI::Texture_Type::Temp0,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(ALBEDO)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp1,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(DIFFUSE)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp2,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(LIGHT_SHADOW)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp3,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_2)]->GetRTTexture(static_cast<int>(UP2X2_2)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp4,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(UTIL)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp5,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(AMBIENT)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp6,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(SPECULAR_LIGHT)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp7,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::SILHOUETTE)]->GetRTTexture(static_cast<int>(SILHOUETTE)).get());
    }

    // BackBuffer
    {
        yunuGI::IMaterial* material = CrateMaterial(L"BackBufferMaterial");
        material->SetVertexShader(GetDeferredShader(L"Deferred_FinalVS.cso").get());
        material->SetPixelShader(GetDeferredShader(L"BackBufferPS.cso").get());
        material->SetTexture(yunuGI::Texture_Type::Temp0,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::FINAL)]->GetRTTexture(static_cast<int>(FINAL)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp1,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(POSITION)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp2,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(OUTLINE_INFO)).get());
        material->SetTexture(yunuGI::Texture_Type::Temp3,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(UTIL)).get());
    }

    // AAMaterial -> 스왑체인의 백버퍼에 기록될 때 사용될 머터리얼
    {
        yunuGI::IMaterial* material = CrateMaterial(L"AAMaterial");
        material->SetVertexShader(GetDeferredShader(L"Deferred_FinalVS.cso").get());
        material->SetPixelShader(GetShader(L"AAPassPS.cso").get());
        material->SetTexture(yunuGI::Texture_Type::Temp0,
            renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::BACKBUFFER)]->GetRTTexture(static_cast<int>(BACKBUFFER)).get());
    }

    // silhouette material
    {
        yunuGI::IMaterial* material = CrateMaterial(L"SilhouetteMaterial");
        material->SetVertexShader(GetShader(L"SkinnedVS.cso").get());
        material->SetPixelShader(GetShader(L"SilhouettePS.cso").get());
    }

    // 파티클 전용 머터리얼
    {
        yunuGI::IMaterial* material = CrateMaterial(L"ParticleMaterial");
        material->SetVertexShader(GetShader(L"ParticleVS.cso").get());
        material->SetPixelShader(GetShader(L"ParticlePS.cso").get());
        material->SetTexture(yunuGI::Texture_Type::ALBEDO, GetTexture(L"Texture/Particle/default.dds").get());
    }

    /// Deferred Debug Info
    {
        {
            // Position
            yunuGI::IMaterial* material = CrateMaterial(L"DeferredPosition");
            material->SetPixelShader(GetShader(L"TexturePS.cso").get());
            material->SetVertexShader(GetShader(L"TextureVS.cso").get());
            material->SetTexture(yunuGI::Texture_Type::Temp0,
                renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(POSITION)).get());
        }

        {
            // Normal
            yunuGI::IMaterial* material = CrateMaterial(L"DeferredNormal");
            material->SetPixelShader(GetShader(L"TexturePS.cso").get());
            material->SetVertexShader(GetShader(L"TextureVS.cso").get());
            material->SetTexture(yunuGI::Texture_Type::Temp0,
                renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(NORMAL)).get());
        }

        {
            // Color
            yunuGI::IMaterial* material = CrateMaterial(L"DeferredColor");
            material->SetPixelShader(GetShader(L"TexturePS.cso").get());
            material->SetVertexShader(GetShader(L"TextureVS.cso").get());
            material->SetTexture(yunuGI::Texture_Type::Temp0,
                renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(ALBEDO)).get());
        }

        {
            // Depth
            yunuGI::IMaterial* material = CrateMaterial(L"DeferredDepth");
            material->SetPixelShader(GetShader(L"TexturePS.cso").get());
            material->SetVertexShader(GetShader(L"TextureVS.cso").get());
            material->SetTexture(yunuGI::Texture_Type::Temp0,
                renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(UTIL)).get());
        }

        {
            // ARM
            yunuGI::IMaterial* material = CrateMaterial(L"DeferredARM");
            material->SetPixelShader(GetShader(L"TexturePS.cso").get());
            material->SetVertexShader(GetShader(L"TextureVS.cso").get());
            material->SetTexture(yunuGI::Texture_Type::Temp0,
                renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(ARM)).get());
        }

        {
            // DiffuseLight
            yunuGI::IMaterial* material = CrateMaterial(L"DeferredDiffuseLight");
            material->SetPixelShader(GetShader(L"TexturePS.cso").get());
            material->SetVertexShader(GetShader(L"TextureVS.cso").get());
            material->SetTexture(yunuGI::Texture_Type::Temp0,
                renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(DIFFUSE)).get());
        }

        {
            // SpecularLight
            yunuGI::IMaterial* material = CrateMaterial(L"DeferredSpecularLight");
            material->SetPixelShader(GetShader(L"TexturePS.cso").get());
            material->SetVertexShader(GetShader(L"TextureVS.cso").get());
            material->SetTexture(yunuGI::Texture_Type::Temp0,
                renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(LIGHT_SHADOW)).get());
        }

        {
            // Shadow
            yunuGI::IMaterial* material = CrateMaterial(L"DeferredShadow");
            material->SetPixelShader(GetShader(L"TexturePS.cso").get());
            material->SetVertexShader(GetShader(L"TextureVS.cso").get());
            material->SetTexture(yunuGI::Texture_Type::Temp0,
                GetTexture(L"ShadowDepth").get());
        }



        /// PBR
        {
            // SpecularLight
            yunuGI::IMaterial* material = CrateMaterial(L"PBRIrradiance");
            material->SetPixelShader(GetShader(L"TexturePS.cso").get());
            material->SetVertexShader(GetShader(L"TextureVS.cso").get());
            material->SetTexture(yunuGI::Texture_Type::Temp0,
                renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::IRRADIANCE)]->GetRTTexture(static_cast<int>(IRRADIANCE)).get());
        }
    }
}

void ResourceManager::CreateDefaultTexture()
{
    CreateTexture(L"Texture/room.dds");
    CreateTexture(L"Texture/Stage1EnvHDR.dds");
    CreateTexture(L"Texture/Stage1Brdf.dds");
    CreateTexture(L"Texture/Stage1DiffuseHDR.dds");
    CreateTexture(L"Texture/Stage1SpecularHDR.dds");

    CreateTexture(L"Texture/Stage2EnvHDR.dds");
    CreateTexture(L"Texture/Stage2Brdf.dds");
    CreateTexture(L"Texture/Stage2DiffuseHDR.dds");
    CreateTexture(L"Texture/Stage2SpecularHDR.dds");

    CreateTexture(L"Texture/Particle/default.dds");
    CreateTexture(L"Texture/TempTexture.dds");
    CreateTexture(L"Texture/Dissolve.jpg");
    CreateTexture(L"Texture/Stage1LightMap_0.exr");
    CreateTexture(L"Texture/Stage1LightMap_1.exr");
    CreateTexture(L"Texture/Stage1LightMap_2.exr");


    CreateTexture(L"Texture/Stage2LightMap_0.exr");
    CreateTexture(L"Texture/Stage2LightMap_1.exr");
    CreateTexture(L"Texture/Stage2LightMap_2.exr");
    CreateTexture(L"Texture/Stage2LightMap_3.exr");
    CreateTexture(L"Texture/Stage2LightMap_4.exr");
    CreateTexture(L"Texture/Stage2LightMap_5.exr");

    std::shared_ptr<Texture> texture = std::make_shared<Texture>();

    texture->SetName(L"Stage1LightMap");
    std::vector<yunuGI::ITexture*> tempVec;
    tempVec.push_back(GetTexture(L"Texture/Stage1LightMap_0.exr").get());
    tempVec.push_back(GetTexture(L"Texture/Stage1LightMap_1.exr").get());
    tempVec.push_back(GetTexture(L"Texture/Stage1LightMap_2.exr").get());
    texture->CreateLightMapArray(tempVec);
    textureMap.insert({ L"Stage1LightMap", texture });
    textureVec.push_back(texture.get());

    std::shared_ptr<Texture> texture1 = std::make_shared<Texture>();

    texture1->SetName(L"Stage2LightMap");
    std::vector<yunuGI::ITexture*> tempVec1;
    tempVec1.push_back(GetTexture(L"Texture/Stage2LightMap_0.exr").get());
    tempVec1.push_back(GetTexture(L"Texture/Stage2LightMap_1.exr").get());
    tempVec1.push_back(GetTexture(L"Texture/Stage2LightMap_2.exr").get());
    tempVec1.push_back(GetTexture(L"Texture/Stage2LightMap_3.exr").get());
    tempVec1.push_back(GetTexture(L"Texture/Stage2LightMap_4.exr").get());
    tempVec1.push_back(GetTexture(L"Texture/Stage2LightMap_5.exr").get());
    texture1->CreateLightMapArray(tempVec1);
    textureMap.insert({ L"Stage2LightMap", texture1 });
    textureVec.push_back(texture1.get());

    CreateTexture(L"ShadowDepth", SM_SIZE, SM_SIZE, DXGI_FORMAT_R24G8_TYPELESS, static_cast<D3D11_BIND_FLAG>(D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE));

    CreateTexture(L"PointLightShadowDepth", PL_SM_SIZE, PL_SM_SIZE, DXGI_FORMAT_R24G8_TYPELESS, static_cast<D3D11_BIND_FLAG>(D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE), false, 6, MAX_POINT_LIGHT);
}

void ResourceManager::FillFBXData(const std::wstring& fbxName, FBXNode* node, yunuGI::FBXData* fbxData, bool isPreLoad)
{
    if (isPreLoad == false)
    {
        fbxData->nodeName = node->nodeName;
        fbxData->hasAnimation = node->hasAnimation;
        fbxData->child.resize(node->child.size());
        fbxData->materialVec.resize(node->meshVec.size());

        DirectX::SimpleMath::Matrix wtm = (node->worldMatrix);
        DirectX::SimpleMath::Vector3 pos;
        DirectX::SimpleMath::Vector3 scale;
        DirectX::SimpleMath::Quaternion quat;
        wtm.Decompose(scale, quat, pos);
        quat = DirectX::XMQuaternionNormalize(quat);

        fbxData->pos = yunuGI::Vector3{ pos.x, pos.y,pos.z };
        fbxData->scale = yunuGI::Vector3{ scale.x, scale.y,scale.z };
        fbxData->quat = yunuGI::Vector4{ quat.x, quat.y, quat.z, quat.w };

        for (int i = 0; i < node->meshVec.size(); ++i)
        {
            fbxData->meshName = node->meshVec[i].meshName;
            fbxData->aabb.maxPoint = yunuGI::Vector3{ node->meshVec[i].aabb[0].x, node->meshVec[i].aabb[0].y,node->meshVec[i].aabb[0].z };
            fbxData->aabb.minPoint = yunuGI::Vector3{ node->meshVec[i].aabb[1].x, node->meshVec[i].aabb[1].y,node->meshVec[i].aabb[1].z };

            // 실제 Mesh와 Material을 만들자
            if (this->meshMap.find(node->meshVec[i].meshName) == this->meshMap.end())
            {
                std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
                mesh->SetName(node->meshVec[i].meshName);
                mesh->SetData(node->meshVec[i].vertex, node->meshVec[i].indices, node->meshVec[i].aabb[0], node->meshVec[i].aabb[1]);
                this->meshMap.insert({ node->meshVec[i].meshName, mesh });
            }
            else
            {
                if (node->meshVec.size() != 1)
                {
                    std::static_pointer_cast<Mesh>(this->meshMap.find(node->meshVec[i].meshName)->second)
                        ->SetData(node->meshVec[i].vertex, node->meshVec[i].indices, node->meshVec[i].aabb[0], node->meshVec[i].aabb[1]);
                }
            }

            if (this->materialMap.find(node->meshVec[i].material.materialName) == this->materialMap.end())
            {
                std::shared_ptr<Material> material = std::make_shared<Material>();
                material->SetName(node->meshVec[i].material.materialName);

                if (!node->meshVec[i].material.albedoMap.empty())
                {
                    this->CreateTexture(node->meshVec[i].material.albedoMap);
                    material->SetTexture(yunuGI::Texture_Type::ALBEDO, GetTexture(node->meshVec[i].material.albedoMap).get());
                }
                if (!node->meshVec[i].material.armMap.empty())
                {
                    this->CreateTexture(node->meshVec[i].material.armMap);
                    material->SetTexture(yunuGI::Texture_Type::ARM, GetTexture(node->meshVec[i].material.armMap).get());
                }
                if (!node->meshVec[i].material.emissionMap.empty())
                {
                    this->CreateTexture(node->meshVec[i].material.emissionMap);
                    material->SetTexture(yunuGI::Texture_Type::EMISSION, GetTexture(node->meshVec[i].material.emissionMap).get());
                }
                if (!node->meshVec[i].material.normalMap.empty())
                {
                    this->CreateTexture(node->meshVec[i].material.normalMap);
                    material->SetTexture(yunuGI::Texture_Type::NORMAL, GetTexture(node->meshVec[i].material.normalMap).get());
                }
                if (!node->meshVec[i].material.opacityMap.empty())
                {
                    this->CreateTexture(node->meshVec[i].material.opacityMap);
                    material->SetTexture(yunuGI::Texture_Type::OPACITY, GetTexture(node->meshVec[i].material.opacityMap).get());
                }

                fbxData->materialVec[i].materialName = node->meshVec[i].material.materialName;
                fbxData->materialVec[i].albedoMap = node->meshVec[i].material.albedoMap;
                fbxData->materialVec[i].armMap = node->meshVec[i].material.armMap;
                fbxData->materialVec[i].emissionMap = node->meshVec[i].material.emissionMap;
                fbxData->materialVec[i].normalMap = node->meshVec[i].material.normalMap;
                fbxData->materialVec[i].opacityMap = node->meshVec[i].material.opacityMap;

                this->materialMap.insert({ node->meshVec[i].material.materialName , material });
            }
            else
            {
                fbxData->materialVec[i].materialName = node->meshVec[i].material.materialName;
                fbxData->materialVec[i].albedoMap = node->meshVec[i].material.albedoMap;
                fbxData->materialVec[i].armMap = node->meshVec[i].material.armMap;
                fbxData->materialVec[i].emissionMap = node->meshVec[i].material.emissionMap;
                fbxData->materialVec[i].normalMap = node->meshVec[i].material.normalMap;
                fbxData->materialVec[i].opacityMap = node->meshVec[i].material.opacityMap;
            }
        }

        for (int i = 0; i < node->child.size(); ++i)
        {
            fbxData->child[i] = new yunuGI::FBXData;
            this->FillFBXData(fbxName, node->child[i], fbxData->child[i], false);
        }
    }
    else
    {
        for (int i = 0; i < node->meshVec.size(); ++i)
        {
            // 실제 Mesh와 Material을 만들자
            if (this->meshMap.find(node->meshVec[i].meshName) == this->meshMap.end())
            {
                std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
                mesh->SetName(node->meshVec[i].meshName);
                mesh->SetData(node->meshVec[i].vertex, node->meshVec[i].indices, node->meshVec[i].aabb[0], node->meshVec[i].aabb[1]);

                mesh->SetDiffuseExposure(fbxData->diffuseExposure);
                mesh->SetAmbientExposure(fbxData->ambientExposure);
                mesh->SetCastDecal(fbxData->castDecal);
                mesh->SetCalculateShadow(fbxData->calculateShadow);

                this->meshMap.insert({ node->meshVec[i].meshName, mesh });
            }
            else
            {
                if (node->meshVec.size() != 1)
                {
                    std::static_pointer_cast<Mesh>(this->meshMap.find(node->meshVec[i].meshName)->second)
                        ->SetData(node->meshVec[i].vertex, node->meshVec[i].indices, node->meshVec[i].aabb[0], node->meshVec[i].aabb[1]);
                }
            }

            if (this->materialMap.find(fbxData->materialVec[i].materialName) == this->materialMap.end())
            {
                std::shared_ptr<Material> material = std::make_shared<Material>();
                material->SetName(fbxData->materialVec[i].materialName);

                if (!fbxData->materialVec[i].albedoMap.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].albedoMap);
                    material->SetTexture(yunuGI::Texture_Type::ALBEDO, GetTexture(fbxData->materialVec[i].albedoMap).get());
                }
                if (!fbxData->materialVec[i].armMap.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].armMap);
                    material->SetTexture(yunuGI::Texture_Type::ARM, GetTexture(fbxData->materialVec[i].armMap).get());
                }
                if (!fbxData->materialVec[i].emissionMap.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].emissionMap);
                    material->SetTexture(yunuGI::Texture_Type::EMISSION, GetTexture(fbxData->materialVec[i].emissionMap).get());
                }
                if (!fbxData->materialVec[i].normalMap.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].normalMap);
                    material->SetTexture(yunuGI::Texture_Type::NORMAL, GetTexture(fbxData->materialVec[i].normalMap).get());
                }
                if (!fbxData->materialVec[i].opacityMap.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].opacityMap);
                    material->SetTexture(yunuGI::Texture_Type::OPACITY, GetTexture(fbxData->materialVec[i].opacityMap).get());
                }
                if (!fbxData->materialVec[i].heightMap.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].heightMap);
                    material->SetTexture(yunuGI::Texture_Type::HEIGHT, GetTexture(fbxData->materialVec[i].heightMap).get());
                }
                if (!fbxData->materialVec[i].temp0Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp0Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp0, GetTexture(fbxData->materialVec[i].temp0Map).get());
                }
                if (!fbxData->materialVec[i].temp1Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp1Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp1, GetTexture(fbxData->materialVec[i].temp1Map).get());
                }
                if (!fbxData->materialVec[i].temp2Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp2Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp2, GetTexture(fbxData->materialVec[i].temp2Map).get());
                }
                if (!fbxData->materialVec[i].temp3Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp3Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp3, GetTexture(fbxData->materialVec[i].temp3Map).get());
                }
                if (!fbxData->materialVec[i].temp4Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp4Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp4, GetTexture(fbxData->materialVec[i].temp4Map).get());
                }
                if (!fbxData->materialVec[i].temp5Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp5Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp5, GetTexture(fbxData->materialVec[i].temp5Map).get());
                }
                if (!fbxData->materialVec[i].temp6Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp6Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp6, GetTexture(fbxData->materialVec[i].temp6Map).get());
                }
                if (!fbxData->materialVec[i].temp7Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp7Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp7, GetTexture(fbxData->materialVec[i].temp7Map).get());
                }
                if (!fbxData->materialVec[i].temp8Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp8Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp8, GetTexture(fbxData->materialVec[i].temp8Map).get());
                }
                if (!fbxData->materialVec[i].temp9Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp9Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp9, GetTexture(fbxData->materialVec[i].temp9Map).get());
                }
                if (!fbxData->materialVec[i].temp10Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp10Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp10, GetTexture(fbxData->materialVec[i].temp10Map).get());
                }
                if (!fbxData->materialVec[i].temp11Map.empty())
                {
                    this->CreateTexture(fbxData->materialVec[i].temp11Map);
                    material->SetTexture(yunuGI::Texture_Type::Temp11, GetTexture(fbxData->materialVec[i].temp11Map).get());
                }

                if (!fbxData->materialVec[i].vs.empty())
                {
                    auto shader = GetShader(fbxData->materialVec[i].vs);
                    material->SetVertexShader(shader.get());
                }
                if (!fbxData->materialVec[i].ps.empty())
                {
                    auto shader = GetShader(fbxData->materialVec[i].ps);
                    material->SetPixelShader(shader.get());
                }

                this->materialMap.insert({ node->meshVec[i].material.materialName , material });
            }
            else
            {
                fbxData->materialVec[i].materialName = node->meshVec[i].material.materialName;
                fbxData->materialVec[i].albedoMap = node->meshVec[i].material.albedoMap;
                fbxData->materialVec[i].armMap = node->meshVec[i].material.armMap;
                fbxData->materialVec[i].emissionMap = node->meshVec[i].material.emissionMap;
                fbxData->materialVec[i].normalMap = node->meshVec[i].material.normalMap;
                fbxData->materialVec[i].opacityMap = node->meshVec[i].material.opacityMap;
            }
        }
    }

    for (int i = 0; i < node->child.size(); ++i)
    {
        this->FillFBXData(fbxName, node->child[i], fbxData->child[i], true);
    }
}

//void ResourceManager::FillFBXData(const std::wstring& fbxName, FBXNode& node, std::vector<yunuGI::FBXData>& dataVec)
//{
//	//for (int i = 0; i < node.meshVec.size(); ++i)
//	//{
//	//	auto iter = this->meshMap.find(node.meshVec[i].meshName);
//
//	//	// 이미 메쉬 데이터가 있다면 이 메쉬는 다중 머터리얼임
//	//	if (iter != this->meshMap.end())
//	//	{
//	//		std::static_pointer_cast<Mesh>(iter->second)->SetData(node.meshVec[i].vertex, node.meshVec[i].indices);
//
//	//		for (auto& e : this->fbxDataVecMap.find(fbxName)->second)
//	//		{
//	//			if (e.meshName != node.meshVec[i].meshName) continue;
//
//	//			CreateResourceFromFBX(node.meshVec[i], this->fbxDataVecMap.find(fbxName)->second, e);
//	//		}
//	//	}
//	//	else
//	//	{
//	//		// 메쉬 데이터가 없다면 메쉬를 생성 후 삽입
//	//		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
//	//		mesh->SetName(node.meshVec[i].meshName);
//	//		mesh->SetData(node.meshVec[i].vertex, node.meshVec[i].indices);
//	//		CreateMesh(mesh);
//
//	//		yunuGI::FBXData data;
//	//		data.hasAnimation = node.hasAnimation;
//	//		CreateResourceFromFBX(node.meshVec[i], dataVec,  data);
//	//	}
//	//}
//
//	//for (int i = 0; i < node.child.size(); ++i)
//	//{
//	//	FillFBXData(fbxName, node.child[i], dataVec);
//	//}
//}

void ResourceManager::FillFBXBoneInfoVec(const yunuGI::BoneInfo& boneInfo, std::vector<yunuGI::BoneInfo>& boneInfoVec)
{
    boneInfoVec.emplace_back(boneInfo);

    //for (int i = 0; i < boneInfo.child.size(); ++i)
    //{
    //	FillFBXBoneInfoVec(boneInfo.child[i], boneInfoVec);
    //}
}

//void ResourceManager::CreateResourceFromFBX(FBXMeshData& meshData, std::vector<yunuGI::FBXData>& dataVec, yunuGI::FBXData& fbxData)
//{
//	// Fill FBXData 
//	yunuGI::FBXData _data;
//	// 1. Mesh Name Set
//	_data.meshName = meshData.meshName;
//	// 2. Material Set
//	yunuGI::MaterialData materialData;
//	materialData.materialName = meshData.material.materialName;
//	materialData.albedoMap = meshData.material.albedoMap;
//	materialData.normalMap = meshData.material.normalMap;
//	materialData.armMap = meshData.material.armMap;
//	materialData.emissionMap = meshData.material.emissionMap;
//	_data.materialVec.emplace_back(materialData);
//	// Data Push
//	if (fbxData.meshName.empty())
//	{
//		// 메쉬가 새로 생성 되었을 경우
//		dataVec.emplace_back(_data);
//	}
//	else
//	{
//		// 이미 메쉬가 있는 경우 머터리얼만 추가
//		fbxData.materialVec.emplace_back(materialData);
//	}
//
//	// Create Texture
//	CreateTexture(materialData.albedoMap);
//	CreateTexture(materialData.normalMap);
//	CreateTexture(materialData.armMap);
//	CreateTexture(materialData.emissionMap);
//
//	// Create Material
//	if (this->GetMaterial(materialData.materialName))
//	{
//		return;
//	}
//	else
//	{
//		yunuGI::IMaterial* material = CrateMaterial(materialData.materialName);
//		material->SetTexture(yunuGI::Texture_Type::ALBEDO, GetTexture(materialData.albedoMap).get());
//		material->SetTexture(yunuGI::Texture_Type::NORMAL, GetTexture(materialData.normalMap).get());
//		material->SetTexture(yunuGI::Texture_Type::ARM, GetTexture(materialData.armMap).get());
//		material->SetTexture(yunuGI::Texture_Type::EMISSION, GetTexture(materialData.emissionMap).get());
//	}
//}

//void ResourceManager::PushFBXBoneInfo(const std::wstring fbxName, std::vector<FBXBoneInfo> fbxBoneInfo)
//{
//	this->BoneOffsetInfoVecMap.insert({ fbxName, fbxBoneInfo });
//}

//void ResourceManager::LoadCubeMesh()
//{
//	std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>();
//
//	cubeMesh->SetName(L"Cube");
//
//	float w2 = 0.5f;
//	float h2 = 0.5f;
//	float d2 = 0.5f;
//
//	std::vector<Vertex> vec(24);
//
//	// 앞면
//	vec[0] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
//	vec[1] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
//	vec[2] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
//	vec[3] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
//	// 뒷면
//	vec[4] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
//	vec[5] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
//	vec[6] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
//	vec[7] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
//	// 윗면
//	vec[8] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
//	vec[9] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
//	vec[10] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.f, 0.f), DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
//	vec[11] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.f, 1.f), DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
//	// 아랫면
//	vec[12] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
//	vec[13] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
//	vec[14] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
//	vec[15] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
//	// 왼쪽면
//	vec[16] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f));
//	vec[17] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f));
//	vec[18] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f));
//	vec[19] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f));
//	// 오른쪽면
//	vec[20] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
//	vec[21] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
//	vec[22] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
//	vec[23] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
//
//	std::vector<unsigned int> idx(36);
//
//	// 앞면
//	idx[0] = 0; idx[1] = 1; idx[2] = 2;
//	idx[3] = 0; idx[4] = 2; idx[5] = 3;
//	// 뒷면
//	idx[6] = 4; idx[7] = 5; idx[8] = 6;
//	idx[9] = 4; idx[10] = 6; idx[11] = 7;
//	// 윗면
//	idx[12] = 8; idx[13] = 9; idx[14] = 10;
//	idx[15] = 8; idx[16] = 10; idx[17] = 11;
//	// 아랫면
//	idx[18] = 12; idx[19] = 13; idx[20] = 14;
//	idx[21] = 12; idx[22] = 14; idx[23] = 15;
//	// 왼쪽면
//	idx[24] = 16; idx[25] = 17; idx[26] = 18;
//	idx[27] = 16; idx[28] = 18; idx[29] = 19;
//	// 오른쪽면
//	idx[30] = 20; idx[31] = 21; idx[32] = 22;
//	idx[33] = 20; idx[34] = 22; idx[35] = 23;
//
//
//
//	DirectX::SimpleMath::Vector3 minPoint = vec[0].pos;
//	DirectX::SimpleMath::Vector3 maxPoint = vec[0].pos;
//
//	for (const auto& vertex : vec) {
//		// minPoint 업데이트
//		minPoint.x = std::min(minPoint.x, vertex.pos.x);
//		minPoint.y = std::min(minPoint.y, vertex.pos.y);
//		minPoint.z = std::min(minPoint.z, vertex.pos.z);
//
//		// maxPoint 업데이트
//		maxPoint.x = max(maxPoint.x, vertex.pos.x);
//		maxPoint.y = max(maxPoint.y, vertex.pos.y);
//		maxPoint.z = max(maxPoint.z, vertex.pos.z);
//	}
//
//
//
//
//	cubeMesh->SetData(vec, idx, maxPoint, minPoint);
//	CreateMesh(cubeMesh);
//}
//
//void ResourceManager::LoadSphereMesh()
//{
//	std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>();
//
//	sphereMesh->SetName(L"Sphere");
//
//	float radius = 0.5f; // 구의 반지름
//	unsigned int stackCount = 20; // 가로 분할
//	unsigned int sliceCount = 20; // 세로 분할
//
//	std::vector<Vertex> vec;
//
//	Vertex v;
//
//	// 북극
//	v.pos = DirectX::SimpleMath::Vector3{ 0.0f, radius, 0.0f };
//	v.color = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 1.0f,1.f };
//	v.uv = DirectX::SimpleMath::Vector2(0.5f, 0.0f);
//	v.normal = v.pos;
//	v.normal.Normalize();
//	v.tangent = DirectX::SimpleMath::Vector3{ 1.f,0.f,1.f };
//	vec.push_back(v);
//
//	float stackAngle = DirectX::XM_PI / stackCount;
//	float sliceAngle = DirectX::XM_2PI / sliceCount;
//
//	float deltaU = 1.f / static_cast<float>(sliceCount);
//	float deltaV = 1.f / static_cast<float>(stackCount);
//
//	// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
//	for (unsigned int y = 1; y <= stackCount - 1; ++y)
//	{
//		float phi = y * stackAngle;
//
//		// 고리에 위치한 정점
//		for (unsigned int x = 0; x <= sliceCount; ++x)
//		{
//			float theta = x * sliceAngle;
//
//			v.pos.x = radius * sinf(phi) * cosf(theta);
//			v.pos.y = radius * cosf(phi);
//			v.pos.z = radius * sinf(phi) * sinf(theta);
//
//			v.uv = DirectX::SimpleMath::Vector2(deltaU * x, deltaV * y);
//
//			v.normal = v.pos;
//			v.normal.Normalize();
//
//			v.tangent.x = -radius * sinf(phi) * sinf(theta);
//			v.tangent.y = 0.0f;
//			v.tangent.z = radius * sinf(phi) * cosf(theta);
//			v.tangent.Normalize();
//
//			v.color = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 1.0f,1.f };
//
//			vec.push_back(v);
//		}
//	}
//
//	// 남극
//	v.pos = DirectX::SimpleMath::Vector3{ 0.0f, -radius, 0.0f };
//	v.color = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 1.0f,1.f };
//	v.uv = DirectX::SimpleMath::Vector2(0.5f, 1.0f);
//	v.normal = v.pos;
//	v.normal.Normalize();
//	v.tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
//	vec.push_back(v);
//
//	std::vector<unsigned int> idx(36);
//
//	// 북극 인덱스
//	for (unsigned int i = 0; i <= sliceCount; ++i)
//	{
//		//  [0]
//		//   |  \
//		//  [i+1]-[i+2]
//		idx.push_back(0);
//		idx.push_back(i + 2);
//		idx.push_back(i + 1);
//	}
//
//	// 몸통 인덱스
//	unsigned int ringVertexCount = sliceCount + 1;
//	for (unsigned int y = 0; y < stackCount - 2; ++y)
//	{
//		for (unsigned int x = 0; x < sliceCount; ++x)
//		{
//			//  [y, x]-[y, x+1]
//			//  |		/
//			//  [y+1, x]
//			idx.push_back(1 + (y)*ringVertexCount + (x));
//			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
//			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
//			//		 [y, x+1]
//			//		 /	  |
//			//  [y+1, x]-[y+1, x+1]
//			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
//			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
//			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
//		}
//	}
//
//	// 남극 인덱스
//	unsigned int bottomIndex = static_cast<unsigned int>(vec.size()) - 1;
//	unsigned int lastRingStartIndex = bottomIndex - ringVertexCount;
//	for (unsigned int i = 0; i < sliceCount; ++i)
//	{
//		//  [last+i]-[last+i+1]
//		//  |      /
//		//  [bottom]
//		idx.push_back(bottomIndex);
//		idx.push_back(lastRingStartIndex + i);
//		idx.push_back(lastRingStartIndex + i + 1);
//	}
//
//
//
//
//	DirectX::SimpleMath::Vector3 minPoint = vec[0].pos;
//	DirectX::SimpleMath::Vector3 maxPoint = vec[0].pos;
//
//	for (const auto& vertex : vec) {
//		// minPoint 업데이트
//		minPoint.x = std::min(minPoint.x, vertex.pos.x);
//		minPoint.y = std::min(minPoint.y, vertex.pos.y);
//		minPoint.z = std::min(minPoint.z, vertex.pos.z);
//
//		// maxPoint 업데이트
//		maxPoint.x = max(maxPoint.x, vertex.pos.x);
//		maxPoint.y = max(maxPoint.y, vertex.pos.y);
//		maxPoint.z = max(maxPoint.z, vertex.pos.z);
//	}
//
//
//
//
//
//	sphereMesh->SetData(vec, idx, maxPoint, minPoint);
//	CreateMesh(sphereMesh);
//}
//
void ResourceManager::LoadRactangleMesh()
{
    rectangleMesh = std::make_shared<Mesh>();

    rectangleMesh->SetName(L"Rectangle");

    float w2 = 0.5f;
    float h2 = 0.5f;

    std::vector<Vertex> vec(4);


    // POS COLOR UV TANGENT
    // 앞면
    vec[0] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
    vec[1] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
    vec[2] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
    vec[3] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));

    std::vector<unsigned int> idx(6);

    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;


    DirectX::SimpleMath::Vector3 minPoint = vec[0].pos;
    DirectX::SimpleMath::Vector3 maxPoint = vec[0].pos;

    for (const auto& vertex : vec) {
        // minPoint 업데이트
        minPoint.x = std::min(minPoint.x, vertex.pos.x);
        minPoint.y = std::min(minPoint.y, vertex.pos.y);
        minPoint.z = std::min(minPoint.z, vertex.pos.z);

        // maxPoint 업데이트
        maxPoint.x = std::max(maxPoint.x, vertex.pos.x);
        maxPoint.y = std::max(maxPoint.y, vertex.pos.y);
        maxPoint.z = std::max(maxPoint.z, vertex.pos.z);
    }


    rectangleMesh->SetData(vec, idx, maxPoint, minPoint);
    CreateMesh(rectangleMesh);
}
//
void ResourceManager::LoadPointMesh()
{
    std::shared_ptr<Mesh> pointMesh = std::make_shared<Mesh>();

    pointMesh->SetName(L"Point");

    std::vector<Vertex> vertices(1);
    vertices[0] = Vertex{ DirectX::SimpleMath::Vector3{0.0f, 0, 0 },
                          DirectX::SimpleMath::Vector4{1.f,1.f,1.f,1.f},
                          DirectX::SimpleMath::Vector2{0.f,0.f},
                          DirectX::SimpleMath::Vector2{0.f,0.f},
                          DirectX::SimpleMath::Vector3{0.0f, 0, -1.f },
                          DirectX::SimpleMath::Vector3{0.0f, 0, -1.f } };

    std::vector<unsigned int> indices(1);

    indices[0] = 0;

    DirectX::SimpleMath::Vector3 minPoint = vertices[0].pos;
    DirectX::SimpleMath::Vector3 maxPoint = vertices[0].pos;

    pointMesh->SetData(vertices, indices, maxPoint, minPoint);
    CreateMesh(pointMesh);
}
//
void ResourceManager::LoadLineMesh()
{
    std::shared_ptr<Mesh> lineMesh = std::make_shared<Mesh>();

    lineMesh->SetName(L"Line");

    std::vector<Vertex> vertices(3);

    vertices[0] = Vertex{ DirectX::SimpleMath::Vector3{0.0f, 0, 0 },
                          DirectX::SimpleMath::Vector4{1.f,1.f,1.f,1.f},
                          DirectX::SimpleMath::Vector2{0.5f,0.5f},
                          DirectX::SimpleMath::Vector2{0.5f,0.5f},
                          DirectX::SimpleMath::Vector3{0.0f, 0, -1.f },
                          DirectX::SimpleMath::Vector3{0.0f, 0, -1.f } };

    vertices[1] = Vertex{ DirectX::SimpleMath::Vector3{1.0f, 0, 0 },
                          DirectX::SimpleMath::Vector4{1.f,1.f,1.f,1.f},
                          DirectX::SimpleMath::Vector2{0.5f,0.5f},
                          DirectX::SimpleMath::Vector2{0.5f,0.5f},
                          DirectX::SimpleMath::Vector3{0.0f, 0, -1.f },
                          DirectX::SimpleMath::Vector3{0.0f, 0, -1.f } };

    vertices[2] = Vertex{ DirectX::SimpleMath::Vector3{1.0f, 0, 0 },
                          DirectX::SimpleMath::Vector4{1.f,1.f,1.f,1.f},
                          DirectX::SimpleMath::Vector2{0.5f,0.5f},
                          DirectX::SimpleMath::Vector2{0.5f,0.5f},
                          DirectX::SimpleMath::Vector3{0.0f, 0, -1.f },
                          DirectX::SimpleMath::Vector3{0.0f, 0, -1.f } };

    std::vector<unsigned int> indices(3);

    indices[0] = 0;
    indices[1] = 1;
    indices[1] = 2;


    DirectX::SimpleMath::Vector3 minPoint = vertices[0].pos;
    DirectX::SimpleMath::Vector3 maxPoint = vertices[0].pos;

    for (const auto& vertex : vertices) {
        // minPoint 업데이트
        minPoint.x = std::min(minPoint.x, vertex.pos.x);
        minPoint.y = std::min(minPoint.y, vertex.pos.y);
        minPoint.z = std::min(minPoint.z, vertex.pos.z);

        // maxPoint 업데이트
        maxPoint.x = std::max(maxPoint.x, vertex.pos.x);
        maxPoint.y = std::max(maxPoint.y, vertex.pos.y);
        maxPoint.z = std::max(maxPoint.z, vertex.pos.z);
    }


    lineMesh->SetData(vertices, indices, maxPoint, minPoint);
    CreateMesh(lineMesh);
}
//
//void ResourceManager::LoadCapsuleMesh()
//{
//	std::shared_ptr<Mesh> capsuleMesh = std::make_shared<Mesh>();
//
//	capsuleMesh->SetName(L"Capsule");
//
//	float radius = 0.5f; // 캡슐의 반지름
//	float height = 1.0f; // 캡슐의 높이
//	int stackCount = 5; // 수평 분할
//	int sliceCount = 20; // 수직 분할
//
//	std::vector<Vertex> vertices;
//	std::vector<unsigned int> indices;
//
//	// 상단 반구 정점
//	vertices.push_back(Vertex{
//		DirectX::SimpleMath::Vector3{0.0f, radius + height * 0.5f, 0.0f},
//		DirectX::SimpleMath::Vector4{1.f, 1.f, 1.f, 1.f} });
//
//	for (int i = 1; i <= stackCount; i++)
//	{
//		// 윗방향 벡터와의 각도
//		float upTheta = DirectX::XM_PI * 0.5f * (i / static_cast<float>(stackCount));
//
//		float xzsize = radius * sinf(upTheta);
//		float ysize = radius * cosf(upTheta);
//
//		for (int j = 0; j < sliceCount; j++)
//		{
//			float zTheta = DirectX::XM_PI * 2.0f * (j / static_cast<float>(sliceCount));
//
//			float x = xzsize * sinf(zTheta);
//			float y = ysize + height * 0.5f;
//			float z = xzsize * cosf(zTheta);
//
//			vertices.push_back(Vertex{
//				DirectX::SimpleMath::Vector3{x, y, z},
//				DirectX::SimpleMath::Vector4(1.f, 1.f,1.f,1.f) }
//			);
//		}
//	}
//
//	size_t middleIdx = vertices.size();
//
//	// 하단 반구 정점
//	for (int i = stackCount; i >= 1; i--)
//	{
//		// 윗방향 벡터와의 각도
//		float upTheta = DirectX::XM_PI * 0.5f * (i / static_cast<float>(stackCount));
//
//		float xzsize = radius * sinf(upTheta);
//		float ysize = radius * cosf(upTheta);
//
//		for (int j = 0; j < sliceCount; j++)
//		{
//			float zTheta = DirectX::XM_PI * 2.0f * (j / static_cast<float>(sliceCount));
//
//			float x = xzsize * sinf(zTheta);
//			float y = ysize + height * 0.5f;
//			float z = xzsize * cosf(zTheta);
//
//			vertices.push_back(Vertex{
//				DirectX::SimpleMath::Vector3(x, -y, z),
//				DirectX::SimpleMath::Vector4(1.f, 1.f,1.f,1.f) }
//			);
//		}
//	}
//
//	vertices.push_back(Vertex{
//		DirectX::SimpleMath::Vector3{0.0f, -(radius + height * 0.5f), 0.0f},
//		DirectX::SimpleMath::Vector4(1.f, 1.f,1.f,1.f) }
//	);
//
//	// 상단 반구 인덱스
//	for (int i = 0; i < sliceCount; i++) {
//		int a = 0;
//		int b = 1 + i;
//		int c = 1 + ((i + 1) % sliceCount);
//
//		indices.push_back(a);
//		indices.push_back(b);
//		indices.push_back(c);
//	}
//
//	for (int i = 1; i < stackCount; i++) {
//		for (int j = 0; j < sliceCount; j++) {
//			int a = 1 + (i - 1) * sliceCount + j;
//			int b = 1 + (i - 1) * sliceCount + ((j + 1) % sliceCount);
//			int c = 1 + i * sliceCount + j;
//			int d = 1 + i * sliceCount + ((j + 1) % sliceCount);
//
//			indices.push_back(a);
//			indices.push_back(c);
//			indices.push_back(d);
//
//			indices.push_back(a);
//			indices.push_back(d);
//			indices.push_back(b);
//		}
//	}
//
//	// 실린더 부분 인덱스
//	for (int i = 0; i < sliceCount; i++)
//	{
//		int a = middleIdx - sliceCount + i;
//		int b = middleIdx - sliceCount + ((i + 1) % sliceCount);
//		int c = middleIdx + i;
//		int d = middleIdx + ((i + 1) % sliceCount);
//
//		indices.push_back(a);
//		indices.push_back(c);
//		indices.push_back(d);
//
//		indices.push_back(a);
//		indices.push_back(d);
//		indices.push_back(b);
//	}
//
//	// 하단 반구 인덱스
//	for (int i = 1; i < stackCount; i++) {
//		for (int j = 0; j < sliceCount; j++) {
//			int a = middleIdx + (i - 1) * sliceCount + j;
//			int b = middleIdx + (i - 1) * sliceCount + ((j + 1) % sliceCount);
//			int c = middleIdx + i * sliceCount + j;
//			int d = middleIdx + i * sliceCount + ((j + 1) % sliceCount);
//
//			indices.push_back(a);
//			indices.push_back(c);
//			indices.push_back(d);
//
//			indices.push_back(a);
//			indices.push_back(d);
//			indices.push_back(b);
//		}
//	}
//
//	for (int i = 0; i < sliceCount; i++) {
//		int a = vertices.size() - 1;
//		int b = vertices.size() - 1 - sliceCount + i;
//		int c = vertices.size() - 1 - sliceCount + ((i + 1) % sliceCount);
//
//		indices.push_back(b);
//		indices.push_back(a);
//		indices.push_back(c);
//	}
//
//
//
//	DirectX::SimpleMath::Vector3 minPoint = vertices[0].pos;
//	DirectX::SimpleMath::Vector3 maxPoint = vertices[0].pos;
//
//	for (const auto& vertex : vertices) {
//		// minPoint 업데이트
//		minPoint.x = std::min(minPoint.x, vertex.pos.x);
//		minPoint.y = std::min(minPoint.y, vertex.pos.y);
//		minPoint.z = std::min(minPoint.z, vertex.pos.z);
//
//		// maxPoint 업데이트
//		maxPoint.x = max(maxPoint.x, vertex.pos.x);
//		maxPoint.y = max(maxPoint.y, vertex.pos.y);
//		maxPoint.z = max(maxPoint.z, vertex.pos.z);
//	}
//
//
//	capsuleMesh->SetData(vertices, indices, maxPoint, minPoint);
//	CreateMesh(capsuleMesh);
//}
//
//void ResourceManager::LoadCylinderMesh()
//{
//	std::shared_ptr<Mesh> cylinderMesh = std::make_shared<Mesh>();
//
//	cylinderMesh->SetName(L"Cylinder");
//
//	std::vector<Vertex> vertices;
//	std::vector<unsigned int> indices;
//
//	float radius = 0.5f; // 실린더의 반지름
//	float height = 1.0f; // 실린더의 높이
//	int sliceCount = 20; // 수직 분할
//
//	// 정상 정점
//	vertices.push_back(Vertex{
//		DirectX::SimpleMath::Vector3{0.0f, height * 0.5f, 0.0f},
//		DirectX::SimpleMath::Vector4{1.f, 1.f,1.f,1.f},
//		DirectX::SimpleMath::Vector2{ 0.0f, 1.0f },
//		DirectX::SimpleMath::Vector3{ 0.0f, 1.0f, 0.0f } }
//	);
//
//	// 바닥 정점
//	vertices.push_back(Vertex{
//		DirectX::SimpleMath::Vector3{0.0f, -height * 0.5f, 0.0f},
//		DirectX::SimpleMath::Vector4{1.f, 1.f, 1.f,1.f} ,
//		DirectX::SimpleMath::Vector2{ 0.0f, 1.0f },
//		DirectX::SimpleMath::Vector3{ 0.0f, 1.0f, 0.0f } }
//	);
//
//	// 윗면
//	for (int i = 0; i < sliceCount; ++i)
//	{
//		float zTheta = DirectX::XM_PI * 2.0f * (i / static_cast<float>(sliceCount));
//
//		float x = radius * sinf(zTheta);
//		float y = height * 0.5f;
//		float z = radius * cosf(zTheta);
//
//		vertices.push_back(Vertex{
//			DirectX::SimpleMath::Vector3{x, y, z},
//			DirectX::SimpleMath::Vector4{1.f, 1.f,1.f,1.f},
//			DirectX::SimpleMath::Vector2{ 0.0f, 1.0f },
//			DirectX::SimpleMath::Vector3{ 0.0f, 1.0f, 0.0f } }
//		);
//	}
//
//	// 밑면
//	for (int i = 0; i < sliceCount; ++i)
//	{
//		float zTheta = DirectX::XM_PI * 2.0f * (i / static_cast<float>(sliceCount));
//
//		float x = radius * sinf(zTheta);
//		float y = height * 0.5f;
//		float z = radius * cosf(zTheta);
//
//		vertices.push_back(Vertex{
//			DirectX::SimpleMath::Vector3{x, -y, z},
//			DirectX::SimpleMath::Vector4{1.f, 1.f,1.f,1.f },
//			DirectX::SimpleMath::Vector2{ 0.0f, 1.0f },
//			DirectX::SimpleMath::Vector3{ 0.0f, 1.0f, 0.0f } }
//		);
//	}
//
//	// 윗면 인덱스
//	for (int i = 0; i < sliceCount; ++i)
//	{
//		int a = 0;
//		int b = 2 + i;
//		int c = 2 + ((i + 1) % sliceCount);
//
//		indices.push_back(a);
//		indices.push_back(b);
//		indices.push_back(c);
//	}
//
//	// 옆면 인덱스
//	for (int i = 0; i < sliceCount; ++i)
//	{
//		int a = 2 + i;
//		int b = 2 + ((i + 1) % sliceCount);
//		int c = 2 + sliceCount + i;
//		int d = 2 + sliceCount + ((i + 1) % sliceCount);
//
//		indices.push_back(a);
//		indices.push_back(c);
//		indices.push_back(d);
//
//		indices.push_back(a);
//		indices.push_back(d);
//		indices.push_back(b);
//	}
//
//	// 밑면 인덱스
//	for (int i = 0; i < sliceCount; ++i)
//	{
//		int a = 1;
//		int b = 2 + sliceCount + i;
//		int c = 2 + sliceCount + ((i + 1) % sliceCount);
//
//		indices.push_back(b);
//		indices.push_back(a);
//		indices.push_back(c);
//	}
//
//
//
//
//	DirectX::SimpleMath::Vector3 minPoint = vertices[0].pos;
//	DirectX::SimpleMath::Vector3 maxPoint = vertices[0].pos;
//
//	for (const auto& vertex : vertices) {
//		// minPoint 업데이트
//		minPoint.x = std::min(minPoint.x, vertex.pos.x);
//		minPoint.y = std::min(minPoint.y, vertex.pos.y);
//		minPoint.z = std::min(minPoint.z, vertex.pos.z);
//
//		// maxPoint 업데이트
//		maxPoint.x = max(maxPoint.x, vertex.pos.x);
//		maxPoint.y = max(maxPoint.y, vertex.pos.y);
//		maxPoint.z = max(maxPoint.z, vertex.pos.z);
//	}
//
//
//
//	cylinderMesh->SetData(vertices, indices, maxPoint, minPoint);
//	CreateMesh(cylinderMesh);
//}
//
//void ResourceManager::LoadTriangleMesh()
//{
//	std::shared_ptr<Mesh> triangleMesh = std::make_shared<Mesh>();
//
//	triangleMesh->SetName(L"Triangle");
//
//	float size = 0.5f;
//
//	std::vector<Vertex> triangleVertices(3);
//
//	// POS COLOR UV TANGENT
//	// 삼각형
//	triangleVertices[0] = Vertex(DirectX::SimpleMath::Vector3(-size, 0.0f, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
//	triangleVertices[1] = Vertex(DirectX::SimpleMath::Vector3(0.0f, size, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.5f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
//	triangleVertices[2] = Vertex(DirectX::SimpleMath::Vector3(size, 0.0f, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
//
//	std::vector<unsigned int> triangleIndices(3);
//
//	triangleIndices[0] = 0; triangleIndices[1] = 1; triangleIndices[2] = 2;
//
//	DirectX::SimpleMath::Vector3 minPoint = triangleVertices[0].pos;
//	DirectX::SimpleMath::Vector3 maxPoint = triangleVertices[0].pos;
//
//	for (const auto& vertex : triangleVertices) {
//		// minPoint 업데이트
//		minPoint.x = std::min(minPoint.x, vertex.pos.x);
//		minPoint.y = std::min(minPoint.y, vertex.pos.y);
//		minPoint.z = std::min(minPoint.z, vertex.pos.z);
//
//		// maxPoint 업데이트
//		maxPoint.x = max(maxPoint.x, vertex.pos.x);
//		maxPoint.y = max(maxPoint.y, vertex.pos.y);
//		maxPoint.z = max(maxPoint.z, vertex.pos.z);
//	}
//
//	triangleMesh->SetData(triangleVertices, triangleIndices, maxPoint, minPoint);
//	CreateMesh(triangleMesh);
//}

std::wstring ResourceManager::String_To_Wstring(const std::string& str)
{
    return std::wstring{ str.begin(), str.end() };
}
