#include "ActorLifecycleManager.h"

#include "ActorAnimationEventSink.h"
#include "ActorDeathEventSink.h"
#include "ActorUnloadEventSink.h"
#include "Utils/BoneUtils.h"
#include "Utils/FreezeManager.h"
#include "Hook.h"
#include "Hook/EquipmentHook.h"

namespace Hook
{
    ActorLifecycleManager& ActorLifecycleManager::GetSingleton()
    {
        static ActorLifecycleManager inst;
        return inst;
    }

    void ActorLifecycleManager::Initialize()
    {
        auto* holder = RE::ScriptEventSourceHolder::GetSingleton();
        if (!holder)
            return;

        holder->AddEventSink(&Hook::ActorDeathEventSink::GetSingleton());
        holder->AddEventSink(&Hook::ActorUnloadEventSink::GetSingleton());

        logger::info("ActorLifecycleManager: global lifecycle events registered");
    }

    void ActorLifecycleManager::OnActorFrozen(const RE::Actor* actor)
    {
        if (!actor) return;

        if (!actor->IsPlayerRef()) return;

        auto formId = actor->GetFormID();

        {
            std::unique_lock lock(_lock); 
            if (!_tracked.insert(formId).second)
                return;
        }

        RegisterAnimationGraph(actor);
    }

    void ActorLifecycleManager::OnActorUnfrozen(const RE::Actor* actor)
    {
        if (!actor) return;

        if (!actor->IsPlayerRef()) return;

        auto formId = actor->GetFormID();

        UnregisterAnimationGraph(actor);

        std::unique_lock lock(_lock);
        _tracked.erase(formId);
    }

    void ActorLifecycleManager::OnActorDestroyed(const RE::Actor* actor)
    {
        if (!actor) return;
            
        if (!actor->IsPlayerRef()) return;

        auto formId = actor->GetFormID();
        // 只做纯数据清理
        {
            std::unique_lock lock(_lock);
            _tracked.erase(formId);
        }

        OnActorUnfrozen(actor);
    }

    void ActorLifecycleManager::OnActor3DLoaded(const RE::Actor* actor)
    {
        if (!actor) {
            return;
        }

        auto* skeleton = Utils::BoneUtils::GetSkeletonRoot(actor);
        if (!skeleton) {
            return;
        }

        // ============================================
        // 1. 同步脚部冻结初始状态（鞋子）
        // ============================================
        if (actor->IsPlayerRef()) {
            Hook::EquipmentHook::SyncInitialFootState(actor);
        }

        // ============================================
        // 2. 预热 / 校验骨骼映射（可选）
        // ============================================
        // Utils::BoneUtils::ValidateSkeleton(skeleton);

        // 这里不直接 Freeze
        // Freeze 是 animation-driven（在 SkeletonHook 里）
    }

    void ActorLifecycleManager::OnActor3DUnloaded(const RE::Actor* actor) {
        if (!actor) return;

        auto _actor = const_cast<RE::Actor*>(actor);

        Utils::FreezeManager::GetSingleton().On3DUnloaded(_actor);
    }

    void ActorLifecycleManager::RegisterAnimationGraph(const RE::Actor* actor)
    {
        Hook::ActorAnimationEventSink::GetSingleton().RegisterForActor(actor);
    }

    void ActorLifecycleManager::UnregisterAnimationGraph(const RE::Actor* actor)
    {
        Hook::ActorAnimationEventSink::GetSingleton().Unregister(actor);
    }
}
