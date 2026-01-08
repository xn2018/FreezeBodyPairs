// Hook/ActorDeathEventSink.cpp

#include "ActorUnloadEventSink.h"
#include "SkeletonHook.h"
#include "Utils/BoneUtils.h"

namespace Hook
{
    ActorUnloadEventSink& ActorUnloadEventSink::GetSingleton()
    {
        static ActorUnloadEventSink inst;
        return inst;
    }

    RE::BSEventNotifyControl ActorUnloadEventSink::ProcessEvent(const RE::TESObjectLoadedEvent* evn,
                                                                RE::BSTEventSource<RE::TESObjectLoadedEvent>*) {
        if (!evn || evn->loaded) return RE::BSEventNotifyControl::kContinue;

        auto* ref = RE::TESForm::LookupByID(evn->formID);
        auto* actor = ref ? ref->As<RE::Actor>() : nullptr;
        if (!actor) return RE::BSEventNotifyControl::kContinue;

        Utils::BoneUtils::ClearCachedTransforms(actor);
        Hook::SetFreezeActor(actor, false);

        return RE::BSEventNotifyControl::kContinue;
    }
}
