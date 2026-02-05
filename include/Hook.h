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
    static bool g_frozen = false;

	static void hkbClipGenerator_Generate(
		RE::hkbClipGenerator* a_this, 
		const RE::hkbContext& a_context,
		const RE::hkbGeneratorOutput** a_activeChildrenOutput,
		RE::hkbGeneratorOutput& a_output, 
		float a_timeOffset);

    using EquipObject_t = void (*)(RE::Actor*, RE::TESBoundObject*, RE::ExtraDataList*, std::uint32_t,
                                   const RE::BGSEquipSlot*, bool, bool, bool, bool);

    using UnequipObject_t = bool (*)(RE::Actor*, RE::TESBoundObject*, RE::ExtraDataList*, std::uint32_t,
                                     const RE::BGSEquipSlot*, bool, bool, bool, bool, const RE::BGSEquipSlot*);
	struct HkbGeneratorOutput_Runtime {
        void* vtbl;               // 00
        std::uint64_t unk08;      // 08
        std::uint64_t unk10;      // 10
        RE::hkQsTransform* pose;  // 18 or 20 / 30 (依版本)
    };
	
    void Install();
    void ToggleFootFreeze(RE::PlayerCharacter* player, bool frozen);
    static RE::Actor* GetActorFromContext(const RE::hkbContext& ctx);

    // Hook PlayerCharacter virtual functions for better performance
    class PlayerCharacterHook {
    public:
        static void Install();

    private:
        static void Update(RE::PlayerCharacter* a_this, float a_delta);
        static void UpdateAnimation(RE::PlayerCharacter* a_this, float a_delta);

        static inline REL::Relocation<decltype(Update)> _Update;
        static inline REL::Relocation<decltype(UpdateAnimation)> _UpdateAnimation;
    };
}
