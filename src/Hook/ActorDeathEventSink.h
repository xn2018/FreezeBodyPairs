// Hook/ActorDeathEventSink.h
#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

namespace Hook
{
    class ActorDeathEventSink :
        public RE::BSTEventSink<RE::TESDeathEvent>
    {
    public:
        static ActorDeathEventSink& GetSingleton();
        RE::BSEventNotifyControl ProcessEvent(
            const RE::TESDeathEvent* evn,
            RE::BSTEventSource<RE::TESDeathEvent>*) override;
    };
}
