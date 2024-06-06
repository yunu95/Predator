/// 2024. 06. 05 김상준
/// Skill 에 대한 Manager 클래스
/// Editor 에서 사용하기 위해서 작성(Save / Load)

#pragma once

#include "Singleton.h"
#include "Storable.h"
#include "Identifiable.h"
#include "Skill.h"
#include "SkillType.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace application
{
    namespace editor
    {
        class SkillManager
            : public Storable, public Singleton<SkillManager>
        {
            friend class Singleton<SkillManager>;
            friend class MapFileManager;

        public:
            /// UUID 저장용 구조체
            struct Skill_Instance
                : public Identifiable, public Storable
            {
                SkillType::Enum type = SkillType::Enum::NONE;

                virtual bool PreEncoding(json& data) const;
                virtual bool PostEncoding(json& data) const;
                virtual bool PreDecoding(const json& data);
                virtual bool PostDecoding(const json& data);
            };

            void Clear();

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            SkillManager();

            std::unordered_map<const UUID, std::shared_ptr<Skill_Instance>> list;
            
        };
    }
}
