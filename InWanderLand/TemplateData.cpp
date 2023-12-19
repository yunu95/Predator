#include "TemplateData.h"

#include "TemplateDataManager.h"

namespace Application
{
    namespace Editor
    {
        std::string TemplateData::GetDataKey() const
        {
            return TemplateDataManager::GetInstance().GetDataKey(this);
        }
    }
}
