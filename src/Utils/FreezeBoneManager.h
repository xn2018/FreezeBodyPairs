#pragma once
#include "RE/B/BShkbAnimationGraph.h"

namespace Utils {
    struct FrozenBoneCache {
        std::unordered_set<std::uint16_t> indices;
        std::uint16_t boneCount;
    };
    class FreezeBoneManager {
    public:
        static FreezeBoneManager* GetSingleton();

        bool IsFrozen(RE::BShkbAnimationGraph* graph, std::uint16_t bone) const;
        void EnsureCache(RE::BShkbAnimationGraph* graph);
        void ClearGraph(RE::BShkbAnimationGraph* graph);
        void ClearAll();

        void SetBoneList(std::vector<std::string> bones);
    private:
        void BuildCache(RE::BShkbAnimationGraph* graph);
        mutable std::mutex _mutex;
        std::unordered_map<RE::BShkbAnimationGraph*, FrozenBoneCache> _graphCache;

        std::vector<std::string> _rootBones;
    };
}
