#include "TemplateData.h"

#include "TemplateDataManager.h"

namespace application
{
    namespace editor
    {
        std::string TemplateData::GetDataKey() const
        {
            return TemplateDataManager::GetInstance().GetDataKey(this);
        }
    }
}
