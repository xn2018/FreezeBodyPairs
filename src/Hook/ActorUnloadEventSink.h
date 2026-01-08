#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

namespace Hook
{
    class ActorUnloadEventSink : public RE::BSTEventSink<RE::TESObjectLoadedEvent> {
    public:
        static ActorUnloadEventSink& GetSingleton();
        RE::BSEventNotifyControl ProcessEvent(const RE::TESObjectLoadedEvent* evn,
                                              RE::BSTEventSource<RE::TESObjectLoadedEvent>*) override;
    };
}