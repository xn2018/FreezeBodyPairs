#pragma once

#include <RE/Skyrim.h>

namespace FreezeBodyPairs {
    bool RegisterHitCounter(RE::BSScript::IVirtualMachine* vm);

    void InitializeHook(SKSE::Trampoline& trampoline);
}