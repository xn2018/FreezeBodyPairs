// Hook/ActorDeathEventSink.cpp

#include "ActorDeathEventSink.h"
#include "Hook.h"
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

        RE::Actor* actor = evn->actorDying->As<RE::Actor>();
        if (!actor)
            return RE::BSEventNotifyControl::kContinue;

        return RE::BSEventNotifyControl::kContinue;
    }
}
