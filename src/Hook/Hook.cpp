#include <Hook.h>

#include <unordered_map>
#include <unordered_set>
#include <mutex>


#include "REL/Relocation.h"
#include "SKSE/Trampoline.h"
#include "RE/N/NiNode.h"
#include "RE/N/NiAVObject.h"
#include "RE/A/Actor.h"
#include "RE/H/hkbClipGenerator.h"
#include "RE/H/hkbBehaviorGraph.h"
#include "RE/B/BShkbAnimationGraph.h"

#include "RE/H/hkVector4.h"

#include "Config.h"
#include "Utils/BoneUtils.h"
#include "Utils/FreezeManager.h"
#include "Logic/ClipBoneMap.h"
#include "Hook/EquipmentHook.h"


using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace Hook
{
    using NiAVObject = RE::NiAVObject;
    using NiNode = RE::NiNode;

    static std::unordered_map<RE::FormID, bool> _actorFrozenMap;
    static std::shared_mutex _mutex;

    static inline REL::Relocation<decltype(hkbClipGenerator_Generate)> _hkbClipGenerator_Generate;
    static inline REL::Relocation<EquipObject_t> _EquipObject;
    static inline REL::Relocation<UnequipObject_t> _UnequipObject;


    static void Hook_EquipObject(RE::Actor* actor, RE::TESBoundObject* object, RE::ExtraDataList* extra, std::uint32_t count,
                          const RE::BGSEquipSlot* slot, bool queue, bool force, bool sound, bool applyNow) {
        
        _EquipObject(actor, object, extra, count, slot, queue, force, sound, applyNow);
        if (actor && actor->IsPlayerRef()) {
            Hook::EquipmentHook::OnEquip(actor, object);
            log::info("heels equiped!!!");
        }  
    }

    static bool Hook_UnequipObject(RE::Actor* actor, RE::TESBoundObject* object, RE::ExtraDataList* extra,
                                   std::uint32_t count,
                            const RE::BGSEquipSlot* slot, bool queue, bool force, bool sound, bool applyNow,
                            const RE::BGSEquipSlot* replaceSlot) {
        
        bool result = _UnequipObject(actor, object, extra, count, slot, queue, force, sound, applyNow, replaceSlot);
        if (actor && actor->IsPlayerRef()) {
            Hook::EquipmentHook::OnUnequip(actor, object);
            log::info("heels unequiped!!!");
        }

        return result;
    }

    void ToggleFootFreeze(RE::PlayerCharacter* player, bool frozen) {
        g_frozen = !frozen;
        if (!player) {
            return;
        }

        auto manager = Utils::BoneUtils::BoneTransformManager::GetSingleton();

        if (g_frozen) {
            // Capture current foot bone positions when freezing
            manager->CaptureFootBones(player);
            RE::DebugNotification("Fronzen Body Pairs: ON");
        } else {
            // Clear captured bones when unfreezing
            manager->Clear();
            RE::DebugNotification("Fronzen Body Pairs: OFF");
        }
    }

    // Hook PlayerCharacter::Update - called every frame before animation update
    void PlayerCharacterHook::Update(RE::PlayerCharacter* a_this, float a_delta) {
        // Call original Update first
        _Update(a_this, a_delta);

        // Note: We restore bones AFTER Update but update is called before UpdateAnimation
        // So we'll handle restoration in UpdateAnimation instead
    }

    // Hook PlayerCharacter::UpdateAnimation - called every frame to update animations
    void PlayerCharacterHook::UpdateAnimation(RE::PlayerCharacter* a_this, float a_delta) {
        // Call original UpdateAnimation first - this updates all bone animations
        _UpdateAnimation(a_this, a_delta);

        // Only process if foot freeze is active
        if (!g_frozen) {
            return;
        }

        // Restore foot bone transforms after animation has updated them
        auto manager = Utils::BoneUtils::BoneTransformManager::GetSingleton();
        if (manager->HasCapturedBones()) {
            manager->RestoreFootBones(a_this);
        }
    }

    // ===== 安装 Hook =====
    void PlayerCharacterHook::Install() {
        //REL::Relocation<uintptr_t> hkbClipGeneratorVtbl{RE::VTABLE_hkbClipGenerator[0]};
        //_hkbClipGenerator_Generate = hkbClipGeneratorVtbl.write_vfunc(0x17, hkbClipGenerator_Generate);

                // Hook PlayerCharacter vtable
        REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{RE::VTABLE_PlayerCharacter[0]};

        // UpdateAnimation is at offset 0x7D (125 decimal)
        _UpdateAnimation = PlayerCharacterVtbl.write_vfunc(0x7D, UpdateAnimation);

        // Update is at offset 0xAD (173 decimal)
        _Update = PlayerCharacterVtbl.write_vfunc(0xAD, Update);
    }
}
