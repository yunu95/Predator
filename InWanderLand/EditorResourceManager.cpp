#include "EditorResourceManager.h"

#include "YunutyEngine.h"
#include "TemplateDataManager.h"
#include "Ornament_TemplateData.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <d3d11.h>

namespace application
{
	namespace editor
	{
        Texture2D::~Texture2D()
        {
            if (textureAddress)
            {
                // DX11 을 기본으로 함
                static_cast<ID3D11ShaderResourceView*>(textureAddress)->Release();
            }
        }

        Texture2D::Texture2D(Texture2D&& texture) noexcept
        {
            this->textureAddress = texture.textureAddress;
            texture.textureAddress = nullptr;
        }

        Texture2D& Texture2D::operator=(Texture2D&& texture) noexcept
        {
            this->textureAddress = texture.textureAddress;
            texture.textureAddress = nullptr;
            return *this;
        }

        void ResourceManager::Initialize(void* device)
        {
            d3dDevice = device;

            // Scene
            LoadTextureFromFile("ImageButtons/Setting.png");
            LoadTextureFromFile("ImageButtons/Scene_Select.png");
            LoadTextureFromFile("ImageButtons/Scene_Move.png");
            LoadTextureFromFile("ImageButtons/Scene_Rotate.png");
            LoadTextureFromFile("ImageButtons/Scene_Scale.png");

            // Terrain
            LoadTextureFromFile("ImageButtons/Terrain_PlaceButton.png");
            LoadTextureFromFile("ImageButtons/Terrain_EraseButton.png");

            // Unit
            LoadTextureFromFile("ImageButtons/Unit_Default.png");
            LoadTextureFromFile("ImageButtons/Unit_AddButton.png");

            // Ornament
            LoadTextureFromFile("ImageButtons/Ornament_Default.png");
            LoadTextureFromFile("ImageButtons/Ornament_AddButton.png");
        }

        Texture2D* ResourceManager::GetTexture2D(std::string filename)
        {
            if (textureMap.find(filename) != textureMap.end())
            {
                return textureMap[filename].get();
            }

            return nullptr;
        }

        bool ResourceManager::LoadTextureFromFile(std::string filename)
        {
            // 기본적으로 Editor\Resources 폴더로부터 상대적 경로를 입력하도록 함
            std::string basePath = "Editor/Resources/";

            // Load from disk into a raw RGBA buffer
            int image_width = 0;
            int image_height = 0;
            unsigned char* image_data = stbi_load((basePath + filename).c_str(), &image_width, &image_height, NULL, 4);
            if (image_data == NULL)
                return false;

            // Create texture
            D3D11_TEXTURE2D_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Width = image_width;
            desc.Height = image_height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;

            ID3D11Texture2D* pTexture = NULL;
            D3D11_SUBRESOURCE_DATA subResource;
            subResource.pSysMem = image_data;
            subResource.SysMemPitch = desc.Width * 4;
            subResource.SysMemSlicePitch = 0;
            static_cast<ID3D11Device*>(d3dDevice)->CreateTexture2D(&desc, &subResource, &pTexture);

            textureMap[filename] = std::make_unique<Texture2D>();

            // Create texture view
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            ZeroMemory(&srvDesc, sizeof(srvDesc));
            srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = desc.MipLevels;
            srvDesc.Texture2D.MostDetailedMip = 0;
            static_cast<ID3D11Device*>(d3dDevice)->CreateShaderResourceView(pTexture, &srvDesc, reinterpret_cast<ID3D11ShaderResourceView**>(textureMap[filename]->GetTextureAddressPointer()));
            pTexture->Release();

            textureMap[filename]->width = image_width;
            textureMap[filename]->height = image_height;
            stbi_image_free(image_data);

            return true;
        }

        void ResourceManager::LoadFbxList()
        {
            //auto fbxList = graphics::Renderer::SingleInstance().GetResourceManager()->GetFbxList();

            /// 획득 이전에 임시 테스트용
            static std::vector<std::string> fbxList;
            fbxList.push_back("SM_Bush_001");
            fbxList.push_back("SM_Bush_002");
            fbxList.push_back("SM_CastleWall");
            fbxList.push_back("SM_CastleWall_Door");
            fbxList.push_back("SM_CastleWall_Pillar");
            fbxList.push_back("SM_Chair");
            fbxList.push_back("SM_Cuptower");
            fbxList.push_back("SM_Fork");
            fbxList.push_back("SM_GuideBook");
            fbxList.push_back("SM_Hat01");
            fbxList.push_back("SM_Hat02");
            fbxList.push_back("SM_SmallBush_001");
            fbxList.push_back("SM_Stone_001");
            fbxList.push_back("SM_Stone_002");
            fbxList.push_back("SM_Stump");
            fbxList.push_back("SM_Temple_Book_etc");
            fbxList.push_back("SM_Temple_Books");
            fbxList.push_back("SM_Temple_Floor");
            fbxList.push_back("SM_Temple_Pillar");
            fbxList.push_back("SM_Temple_Pillar_Broken");
            fbxList.push_back("SM_Temple_Rabbit");
            fbxList.push_back("SM_Temple_Stairs");
            fbxList.push_back("SM_Temple_Welcome");
            fbxList.push_back("SM_Trunk_001");

            auto& tdm = TemplateDataManager::GetSingletonInstance();
            for (auto each : fbxList)
            {
                auto td = tdm.CreateTemplateData<Ornament_TemplateData>(each);
                td->pod.fbxName = each;
                fbxSet.insert(each);
            }
        }
    }
}