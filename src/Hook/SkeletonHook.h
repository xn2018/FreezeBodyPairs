// SkeletonHook.h
#pragma once
#include <mutex>
#include <unordered_set>

#include "RE/Skyrim.h"

namespace Hook {
	struct FrozenActorData {
		RE::NiNode* skeletonRoot{nullptr};
		std::unordered_set<RE::NiNode*> frozenBoneNodes;
	};

	static std::unordered_map<const RE::Actor*, FrozenActorData> g_frozenActors;
	static std::mutex g_mutex;
    inline std::atomic<RE::NiNode*> g_playerRoot{nullptr};

	static void hkbClipGenerator_Generate(
		RE::hkbClipGenerator* a_this, 
		const RE::hkbContext& a_context,
		const RE::hkbGeneratorOutput** a_activeChildrenOutput,
		RE::hkbGeneratorOutput& a_output, 
		float a_timeOffset);

	struct HkbGeneratorOutput_Runtime {
        void* vtbl;               // 00
        std::uint64_t unk08;      // 08
        std::uint64_t unk10;      // 10
        RE::hkQsTransform* pose;  // 18 or 20 / 30 (依版本)
    };

    void Install();
    void SetFreezeActor(const RE::Actor* player, bool isFrozen);
}
