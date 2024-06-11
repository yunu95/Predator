#pragma once
#include "YunutyEngine.h"

namespace application
{
    namespace editor
    {
        class Unit_TemplateData;
        class UnitData;
    }
}
class Unit;

class UnitPool : public SingletonClass<UnitPool>
{
public:
    std::weak_ptr<Unit> Borrow(application::editor::UnitData* data);
    std::weak_ptr<Unit> Borrow(application::editor::Unit_TemplateData* td, const Vector3d& position, float rotation);
    std::weak_ptr<Unit> Borrow(application::editor::Unit_TemplateData* td, const Vector3d& position, const Quaternion& rotation);
    void Return(std::weak_ptr<Unit>);
private:
    std::weak_ptr<Unit> Borrow(application::editor::Unit_TemplateData* td);
    class PoolByTemplate : public GameObjectPool<Unit>
    {
    public:
        application::editor::Unit_TemplateData* templateData{ nullptr };
        virtual void ObjectInitializer(std::weak_ptr<Unit> unit) override;
        virtual void OnBorrow(std::weak_ptr<Unit> unit) override;
    };
    std::unordered_map<const application::editor::Unit_TemplateData*, std::shared_ptr<PoolByTemplate>> poolsByTemplate;
};