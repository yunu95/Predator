#pragma once
#include "YunuGraphicsInterface.h"
#include "IYunuGIRenderable.h"
#include "YGRenderableAdapter.h"

#include "RenderSystem.h"
#include "SKinnedMesh.h"
#include "MaterialWrapper.h"

#include <memory>

namespace yunuGIAdapter
{
    class SkinnedMeshAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::ISkinnedMesh
    {
    public:
        SkinnedMeshAdapter() :RenderableAdapter()
        {
            renderable = std::make_shared<SkinnedMesh>();
            RenderSystem::Instance.Get().PushSkinnedRenderableObject(renderable.get());

            std::shared_ptr<MaterialWrapper> material = std::make_shared<MaterialWrapper>(false, 0);
            material->SetRenderable(this->renderable);
            //renderable->SetMaterial(0, material->GetMaterial());
            this->materialVec.emplace_back(material);
        }

        ~SkinnedMeshAdapter()
        {
            RenderSystem::Instance.Get().PopSkinnedRenderableObject(renderable.get());
        }

        virtual void SetBone(std::wstring fbxName) override
        {
            renderable->SetBone(fbxName);
        }

        virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
        {
            renderable->SetWorldTM(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(worldTM));
        };

        virtual void SetActive(bool isActive)
        {
            renderable->SetActive(isActive);
        };

        virtual void SetMesh(yunuGI::IMesh* mesh)  override
        {
            renderable->SetMesh(reinterpret_cast<Mesh*>(mesh));
        };
        virtual yunuGI::IMesh* GetMesh() const override
        {
            return renderable->GetMesh();
        };

        virtual void SetAnimatorIndex(int animatorIndex) override
        {
            renderable->SetAnimatorIndex(animatorIndex);
        };

        virtual void SetPickingMode(bool isPickingModeOn) {}

        virtual void SetMaterial(unsigned int index, yunuGI::IMaterial* material) override
        {
            // 새로운 Material이라면
            if (index + 1 > this->materialVec.size())
            {
                std::shared_ptr<MaterialWrapper> tempMaterial = std::make_shared<MaterialWrapper>(index);
                tempMaterial->SetRenderable(this->renderable);
                this->materialVec.emplace_back(tempMaterial);

                if (this->materialVec.back()->IsOrigin())
                {
                    this->materialVec.back()->original = reinterpret_cast<Material*>(material);
                }
                else
                {
                    this->materialVec.back()->variation = reinterpret_cast<Material*>(material);
                }
            }
            else
            {
                if (this->materialVec[index]->IsOrigin())
                {
                    this->materialVec[index]->original = reinterpret_cast<Material*>(material);
                }
                else
                {
                    this->materialVec[index]->variation = reinterpret_cast<Material*>(material);
                }
            }

            renderable->SetMaterial(index, material);
        };

        virtual yunuGI::IMaterial* GetMaterial(unsigned int index = 0)override
        {
            return this->materialVec[index]->GetVariation();
        };

    private:
        std::shared_ptr<SkinnedMesh> renderable;
        std::vector<std::shared_ptr<MaterialWrapper>> materialVec;
    };
}
