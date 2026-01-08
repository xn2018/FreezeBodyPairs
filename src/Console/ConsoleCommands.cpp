#include "ConsoleCommands.h"

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "Utils/BoneUtils.h"

using namespace SKSE;
using namespace SKSE::log;

namespace Console
{
    // =========================
    // FreezeBodyPairs.DumpBones
    // =========================
    static bool Cmd_DumpBones(RE::TESObjectREFR*, RE::TESObjectREFR*, RE::FixedStrings)
    {
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) {
            RE::ConsoleLog::GetSingleton()->Print(
                "FreezeBodyPairs: Player not found");
            return false;
        }

        auto root = player->actorDoingPlayerCommand;
        Utils::BoneUtils::SaveActorBones(root);

        RE::ConsoleLog::GetSingleton()->Print("FreezeBodyPairs: ActorBones.json exported");

        return true;
    }

    void Register()
    {
        auto console = RE::ConsoleLog::GetSingleton;
        
        log::info("Console command registered: FreezeBodyPairs.DumpBones");
    }
}
