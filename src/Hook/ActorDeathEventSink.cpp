// Hook/ActorDeathEventSink.cpp

#include "ActorDeathEventSink.h"
#include "SkeletonHook.h"
#include "Utils/BoneUtils.h"

namespace Hook
{
    ActorDeathEventSink& ActorDeathEventSink::GetSingleton()
    {
        static ActorDeathEventSink inst;
        return inst;
    }

    RE::BSEventNotifyControl
    ActorDeathEventSink::ProcessEvent(const RE::TESDeathEvent* evn, RE::BSTEventSource<RE::TESDeathEvent>*)
    {
        if (!evn || !evn->actorDying)
            return RE::BSEventNotifyControl::kContinue;

        const RE::Actor* actor = evn->actorDying->As<RE::Actor>();
        if (!actor)
            return RE::BSEventNotifyControl::kContinue;

        // 清理 Freeze 状态
        Utils::BoneUtils::ClearCachedTransforms(actor);
        Hook::SetFreezeActor(actor, false);

        return RE::BSEventNotifyControl::kContinue;
    }
}
