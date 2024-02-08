/// 2024. 01. 30 김상준
/// 에디터에서 사용할 Resource 를 관리하는 Manager

#pragma once

#include "Singleton.h"

#include "YunuGraphicsInterface.h"

#include "imgui.h"

#include <unordered_map>
#include <unordered_set>
#include <string>

namespace application
{
	namespace editor
	{
		class ResourceManager;

		// Texture 관리를 위한 클래스
		// DX11 의 경우, ShaderResourceView* 형태로 사용됩니다.
		class Texture2D
		{
			friend class ResourceManager;

		public:
			virtual ~Texture2D();

			Texture2D() = default;
			Texture2D(Texture2D&& texture) noexcept;
			Texture2D& operator=(Texture2D&& texture) noexcept;
			Texture2D(const Texture2D&) = delete;
			Texture2D& operator=(const Texture2D&) = delete;

			void* GetID() { return textureAddress; }
			float GetWidth() { return width; }
			float GetHeight() { return height; }

		private:
			void** GetTextureAddressPointer() { return &textureAddress; }
			void* textureAddress = nullptr;
			float width = 0.f;
			float height = 0.f;
		};

		class ResourceManager
			: public Singleton<ResourceManager>
		{
			friend class Singleton<ResourceManager>;

		public:
			void Initialize(void* device);
			void LateInitialize();
			Texture2D* GetTexture2D(std::string filename);
			bool LoadTextureFromFile(std::string filename);
			const std::unordered_set<std::string>& GetFbxList() const { return fbxSet; }
			yunuGI::IShader* GetShader(std::string shaderName);

		private:
			ResourceManager() = default;
			void LoadFbxList();
			void LoadShaderList();

			void* d3dDevice = nullptr;

			// Editor\Resources 폴더로부터 상대적 경로를 Key 값으로 합니다.
			// 해당 Key 값을 통해 LoadTextureFromFile 함수를 호출합니다.
			std::unordered_map<std::string, std::unique_ptr<Texture2D>> textureMap = std::unordered_map<std::string, std::unique_ptr<Texture2D>>();
			// LoadFbxList 함수를 통해 Engine 의 ResourceManager 로부터 FBXList 를 획득합니다.
			// 이때, Ornament_TemplateData 를 Fbx 이름에 맞춰 생성합니다.
			std::unordered_set<std::string> fbxSet = std::unordered_set<std::string>();
			// LoadShaderList 함수를 통해 Engine 의 ResourceManager 로부터 ShaderMap 을 작성합니다.
			// Shader Name 으로부터 IShader 를 획득합니다.
			std::unordered_map<std::string, yunuGI::IShader*> shaderMap = std::unordered_map<std::string, yunuGI::IShader*>();
		};
	}
}

