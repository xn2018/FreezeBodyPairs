// Hook/ActorDeathEventSink.cpp

#include "ActorUnloadEventSink.h"

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

        return RE::BSEventNotifyControl::kContinue;
    }
}
