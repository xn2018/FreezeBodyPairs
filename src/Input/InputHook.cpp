#include "InputHook.h"

#include <string>
#include "Utils/BoneUtils.h"
#include "Config.h"
#include "Hook/SkeletonHook.h"
#include "Hook/ActorLifecycleManager.h"

namespace Input {

    InputEventSink& InputEventSink::GetSingleton() noexcept {
        static InputEventSink inst;
        return inst;
    }

    void InputEventSink::Install() {
        auto dev = RE::BSInputDeviceManager::GetSingleton();
        if (dev) {
            dev->AddEventSink(&InputEventSink::GetSingleton());
            logger::info("InputEventSink installed.");
        }
    }

    void InputEventSink::SetToggleKeys(std::uint32_t key1, std::uint32_t key2) noexcept {
        std::lock_guard<std::mutex> lock(_lock);
        _key1 = key1;
        _key2 = key2;
        _chord.SetKeys(key1, key2);
    }

    void InputEventSink::SetBoneList(const std::vector<std::string>& bones) noexcept {
        std::lock_guard<std::mutex> lock(_lock);
        _frozenBones = bones;
    }

    RE::BSEventNotifyControl InputEventSink::ProcessEvent(RE::InputEvent* const* evns,
                                                          RE::BSTEventSource<RE::InputEvent*>*) {
        
        if (!evns || !*evns) return RE::BSEventNotifyControl::kContinue;

        auto ev = *evns;

        if (ev->GetEventType() != RE::INPUT_EVENT_TYPE::kButton) return RE::BSEventNotifyControl::kContinue;

        auto* btn = ev->AsButtonEvent();
        if (!btn) return RE::BSEventNotifyControl::kContinue;

        bool doToggle = false;
        std::vector<std::string> bonesCopy;
        std::uint32_t id = btn->GetIDCode();

        {
            std::lock_guard<std::mutex> lock(_lock);
            if (_chord.OnButtonEvent(btn)) {
                _frozen = !_frozen;
                doToggle = true;
                bonesCopy = _frozenBones;
            }
        }

        if (doToggle) {
            RE::Actor* player = RE::PlayerCharacter::GetSingleton();
            if (player) {
                Hook::SetFreezeActor(player, _frozen);
                if (_frozen) {
                    Hook::ActorLifecycleManager::GetSingleton().OnActorFrozen(player);
                } else {
                    Hook::ActorLifecycleManager::GetSingleton().OnActorUnfrozen(player);
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }
}