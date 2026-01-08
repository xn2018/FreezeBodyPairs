#include "Utils/FreezeBoneManager.h"
#include "RE/B/BShkbAnimationGraph.h"
#include "Utils//BoneUtils.h"

namespace Utils {
    FreezeBoneManager* FreezeBoneManager::GetSingleton() {
        static FreezeBoneManager instance;
        return std::addressof(instance);
    }

    void FreezeBoneManager::SetBoneList(std::vector<std::string> bones) {
        std::lock_guard<std::mutex> lock(_mutex);
        _rootBones = std::move(bones);
        _graphCache.clear();  // 配置变更 → 全失效
    }

    void FreezeBoneManager::EnsureCache(RE::BShkbAnimationGraph* graph) {
        if (!graph) return;

        {
            std::lock_guard<std::mutex> lock(_mutex);
            auto it = _graphCache.find(graph);
            if (it != _graphCache.end() && it->second.boneCount == graph->numAnimBones) {
                return;  // cache 有效
            }
        }
        
        BuildCache(graph);
    }

    void FreezeBoneManager::BuildCache(RE::BShkbAnimationGraph* graph) {
        FrozenBoneCache cache;
        cache.boneCount = graph->numAnimBones;

        std::function<void(std::uint16_t)> collect;
        collect = [&](std::uint16_t root) {
            if (!cache.indices.insert(root).second) {
                return;
            }

            for (std::uint16_t i = 0; i < graph->numAnimBones; ++i) {
                if (graph->boneNodes[i].node->parentIndex == root) {
                    collect(i);
                }
            }
        };

        for (const auto& name : _rootBones) {
            auto idx = Utils::BoneUtils::FindBoneIndexByName(graph, name);
            if (!idx) {
                logger::warn("FreezeBone: bone '{}' not found", name);
                continue;
            }

            collect(*idx);
        }

        std::lock_guard<std::mutex> lock(_mutex);
        _graphCache[graph] = std::move(cache);

        logger::info("FreezeBone: graph {} frozen bones = {}", fmt::ptr(graph), _graphCache[graph].indices.size());
    }

    bool FreezeBoneManager::IsFrozen(RE::BShkbAnimationGraph* graph, std::uint16_t bone) const {
        std::lock_guard<std::mutex> lock(_mutex);

        auto it = _graphCache.find(graph);
        if (it == _graphCache.end()) {
            return false;
        }

        return it->second.indices.contains(bone);
    }

    void FreezeBoneManager::ClearGraph(RE::BShkbAnimationGraph* graph) {
        std::lock_guard<std::mutex> lock(_mutex);
        _graphCache.erase(graph);
    }

    void FreezeBoneManager::ClearAll() {
        std::lock_guard<std::mutex> lock(_mutex);
        _graphCache.clear();
    }
}
