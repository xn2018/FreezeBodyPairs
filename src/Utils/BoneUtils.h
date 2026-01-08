#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>

#include "RE/A/Actor.h"
#include "RE/N/NiNode.h"

namespace RE {
    class BShkbAnimationGraph;
}

namespace Utils::BoneUtils {
    // ------------------------------------------------------------
    // Bone index helpers (NO STATE, NO CACHE)
    // ------------------------------------------------------------

    std::optional<std::uint16_t> FindBoneIndexByName(RE::BShkbAnimationGraph* graph, std::string_view boneName);

    void CollectChildBones(RE::BShkbAnimationGraph* graph, std::uint16_t root, std::unordered_set<std::uint16_t>& out);
    void ClearCachedTransforms(const RE::Actor* actor);
    void RebuildFrozenBones(const RE::Actor* actor);
    // ------------------------------------------------------------
    // Scene graph helpers
    // ------------------------------------------------------------

    // Best-effort helper: find owning Actor from any NiNode
    RE::Actor* GetActorFromNode(RE::NiNode* node);
}  // namespace Utils::BoneUtils
