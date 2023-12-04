#include "InstanceManager.h"

namespace Application
{
    namespace Editor
    {
        std::unique_ptr<InstanceManager> InstanceManager::instance = nullptr;

        InstanceManager& InstanceManager::GetInstance()
        {
			if (instance == nullptr)
			{
				instance = std::unique_ptr<InstanceManager>(new InstanceManager());
			}

            return *instance;
        }

        InstanceManager::~InstanceManager()
        {

        }

        bool InstanceManager::CreateInstance(const std::string& dataName)
        {
            return false;
        }

        bool InstanceManager::CloneInstance(const std::shared_ptr<EditableData>& prototype)
        {
            return false;
        }

        bool InstanceManager::Save(json& jsonData)
        {
            return false;
        }

        bool InstanceManager::Load(const json& jsonData)
        {
            return false;
        }

        /// private
        InstanceManager::InstanceManager()
        {

        }

        void InstanceManager::Clear()
        {
        }

    }
}
