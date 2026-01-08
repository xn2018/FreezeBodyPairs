#include "ActorLifecycleManager.h"

#include "ActorAnimationEventSink.h"
#include "ActorDeathEventSink.h"
#include "ActorUnloadEventSink.h"
#include "Utils/BoneUtils.h"
#include "SkeletonHook.h"

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
        if (!actor)
            return;

        {
            std::lock_guard<std::mutex> lk(_lock);
            if (!_tracked.insert(actor).second)
                return;
        }

        RegisterAnimationGraph(actor);
    }

    void ActorLifecycleManager::OnActorUnfrozen(const RE::Actor* actor)
    {
        if (!actor)
            return;

        UnregisterAnimationGraph(actor);

        Utils::BoneUtils::ClearCachedTransforms(actor);
        Hook::SetFreezeActor(actor, false);

        std::lock_guard<std::mutex> lk(_lock);
        _tracked.erase(actor);
    }

    void ActorLifecycleManager::OnActorDestroyed(const RE::Actor* actor)
    {
        if (!actor)
            return;

        OnActorUnfrozen(actor);
    }

    void ActorLifecycleManager::OnActor3DLoaded(const RE::Actor* actor)
    {
        if (!actor)
            return;

        Utils::BoneUtils::RebuildFrozenBones(actor);
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
