#pragma once
#include <atomic>

namespace Hook {
    struct FootFreezeState {
        static void SetEnabled(bool a_enable) noexcept { _enabled.store(a_enable, std::memory_order_relaxed); }
        static bool IsEnabled() noexcept { return _enabled.load(std::memory_order_relaxed); }
    private:
        static inline std::atomic_bool _enabled{false};
    };

    class EquipmentHook {
        public:
        static void OnEquip(RE::Actor* actor, RE::TESBoundObject* object);
        static void OnUnequip(RE::Actor* actor, RE::TESBoundObject* object);
        static void SyncInitialFootState(const RE::Actor* actor);
    };
}
