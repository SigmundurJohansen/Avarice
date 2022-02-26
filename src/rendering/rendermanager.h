#pragma once
#include "renderstage.h"
#include "renderpass/renderpass.h"
#include "subrender.h"
#include "pipelines/pipeline.h"
#include "utils/type_info.h"

namespace Avarice
{

    class SubrenderHolder
    {
    public:
        template <typename T, typename... Args>
        T *Add(const Pipeline::Stage &_stage, std::unique_ptr<T> &&_subrender)
        {
            Remove<T>();
            const auto typeId = TypeInfo<Subrender>::GetTypeId<T>();
            m_stages.insert({StageIndex(_stage, m_subrenders.size()), typeId});
            m_subrenders[typeId] = std::move(_subrender);
            return static_cast<T *>(m_subrenders[typeId].get());
        }

        template <typename T>
        void Remove()
        {
            const auto typeId = TypeInfo<Subrender>::GetTypeId<T>();
            RemoveSubrenderStage(typeId);
            m_subrenders.erase(typeId);
        }

        template <typename T>
        T *Get() const
        {
            const auto typeId = TypeInfo<Subrender>::GetTypeId<T>();
            if (auto it = m_subrenders.find(typeId); it != m_subrenders.end() && it->second)
                return static_cast<T *>(it->second.get());

            //throw std::runtime_error("Subrender Holder does not have requested Subrender");
            return nullptr;
        }

        void Clear()
        {
            m_stages.clear();
        }

        void RemoveSubrenderStage(const TypeId &_id)
        {
            for (auto it = m_stages.begin(); it != m_stages.end();)
            {
                if (it->second == _id)
                    it = m_stages.erase(it);
                else
                    ++it;
            }
        }

        void RenderStage(const Pipeline::Stage &_stage, const CommandBuffer &_commandBuffer)
        {
            for (const auto &[stageIndex, typeId] : m_stages)
            {
                if (stageIndex.first != _stage)
                    continue;

                if (auto &subrender = m_subrenders[typeId])
                    if (subrender->IsEnabled())
                        subrender->Render(_commandBuffer);
            }
        }

    private:
        using StageIndex = std::pair<Pipeline::Stage, std::size_t>;
        std::unordered_map<TypeId, std::unique_ptr<Subrender>> m_subrenders;
        std::multimap<StageIndex, TypeId> m_stages;
    };

    class RenderManager
    {
        friend class Graphics;

    public:
        RenderManager() = default;
        virtual ~RenderManager() = default;

        /**
         * Run when switching to this scene from another, use this method to create {@link Subrender}'s.
         */
        virtual void Start() = 0;
        virtual void Update() = 0;

        /**
         * Checks whether a Subrender exists or not.
         * @tparam T The Subrender type.
         * @return If the Subrender has the System.
         */
        /*
        template <typename T>
        bool HasSubrender() const
        {
            return m_subrenderHolder.Has<T>();
        }
        */

        template <typename T>
        T *GetSubrender() const
        {
            return m_subrenderHolder.Get<T>();
        }

        RenderStage *GetRenderStage(uint32_t index) const
        {
            if (m_renderStages.empty() || m_renderStages.size() < index)
                return nullptr;

            return m_renderStages.at(index).get();
        }

    protected:
        template <typename T, typename... Args>
        T *AddSubrender(const Pipeline::Stage &_pipelineStage, Args &&...args)
        {
            return m_subrenderHolder.Add<T>(_pipelineStage, std::make_unique<T>(_pipelineStage, std::forward<Args>(args)...));
        }
        template <typename T>
        void RemoveSubrender()
        {
            m_subrenderHolder.Remove<T>();
        }
        void ClearSubrenders()
        {
            m_subrenderHolder.Clear();
        }

        void AddRenderStage(std::unique_ptr<RenderStage> &&_renderStage)
        {
            m_renderStages.emplace_back(std::move(_renderStage));
        }

    private:
        bool started = false;
        SubrenderHolder m_subrenderHolder;
        std::vector<std::unique_ptr<RenderStage>> m_renderStages;
    };

}
