#include "Hook/EquipmentHook.h"
#include "RE/A/Actor.h"
#include "RE/I/InventoryChanges.h"
#include "RE/T/TESObjectREFR.h"
#include "Logic/ClipBoneMap.h"
#include "Utils/BoneUtils.h"

namespace Hook {
    void EquipmentHook::OnEquip(RE::Actor* actor, RE::TESBoundObject* item) {
        if (!actor || !actor->IsPlayerRef()) {
            return;
        }

        // TODO: 以后在这里判断是否是鞋子
        FootFreezeState::SetEnabled(true);
    }

    void EquipmentHook::OnUnequip(RE::Actor* actor, RE::TESBoundObject* item) {
        if (!actor || !actor->IsPlayerRef()) {
            return;
        }

        // TODO: 判断是否卸下的是鞋子
        FootFreezeState::SetEnabled(false);
    }

    void EquipmentHook::SyncInitialFootState(const RE::Actor* actor) {

        if (!actor->IsPlayerRef()) {
            return;
        }
        auto c_actor = const_cast<RE::Actor*>(actor);
        using Slot = RE::BIPED_MODEL::BipedObjectSlot;

        // Skyrim / NG：脚部装备位
        constexpr Slot kFeetSlot = Slot::kFeet;

        // GetWornArmor 返回 TESObjectARMO*
        auto* wornFeetArmor = c_actor->GetWornArmor(kFeetSlot);

        if (!wornFeetArmor) {
            // 当前没穿鞋
            Hook::FootFreezeState::SetEnabled(false);
            return;
        }

        // 防御性检查：确认它确实是 Feet 槽
        auto* biped = wornFeetArmor->As<RE::BGSBipedObjectForm>();
        if (!biped) {
            Hook::FootFreezeState::SetEnabled(false);
            return;
        }

        const auto mask = biped->GetSlotMask();
        const bool hasFeet = biped->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet);

        Hook::FootFreezeState::SetEnabled(hasFeet);
    }
}
