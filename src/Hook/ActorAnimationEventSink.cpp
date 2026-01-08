#include "Actor3DIndex.h"
#include "ActorAnimationEventSink.h"
#include "Utils/BoneUtils.h"

#include <stack>
#include <fstream>

using namespace SKSE::log;

namespace Hook {
    ActorAnimationEventSink& ActorAnimationEventSink::GetSingleton() noexcept {
        static ActorAnimationEventSink inst;
        return inst;
    }

    void ActorAnimationEventSink::RegisterForActor(const RE::Actor* actor) noexcept {
        if (!actor) return;

        // 事件源在这里
        auto* mutableActor = const_cast<RE::Actor*>(actor);

        auto* holder = mutableActor->As<RE::BShkbAnimationGraph>();
        auto source = holder->GetEventSource<RE::BSAnimationGraphEvent>();

        if (!source) return;

        source->AddEventSink(&ActorAnimationEventSink::GetSingleton());

        logger::info("Registered BSAnimationGraphEvent sink for actor {}", (void*)actor);
    }

    void ActorAnimationEventSink::Unregister(const RE::Actor* actor) noexcept {
        if (!actor) return;

        auto* mutableActor = const_cast<RE::Actor*>(actor);
        auto* holder = mutableActor->As<RE::BShkbAnimationGraph>();
        if (!holder) return;

        auto* source = holder->GetEventSource<RE::BSAnimationGraphEvent>();
        if (!source) return;

        source->RemoveEventSink(this);

        std::lock_guard<std::mutex> lk(g_animMutex);
        _registered.erase(actor);
    }

    RE::BSEventNotifyControl ActorAnimationEventSink::ProcessEvent(const RE::BSAnimationGraphEvent* evn,
                                                                   RE::BSTEventSource<RE::BSAnimationGraphEvent>*) {
        if (!evn) return RE::BSEventNotifyControl::kContinue;

        auto actor = evn->holder ? evn->holder->As<RE::Actor>() : nullptr;
        if (!actor) return RE::BSEventNotifyControl::kContinue;

        if (evn && evn->tag == "On3DLoaded") {
            Utils::BoneUtils::RebuildFrozenBones(actor);
        }

        // Actor 已确认有 Animation Graph，尝试获取其 3D root
        RE::NiAVObject* root = actor->Get3D();
        if (!root) return RE::BSEventNotifyControl::kContinue;

        // 建立反向索引
        Hook::Actor3DIndex::GetSingleton().Register(const_cast<RE::Actor*>(actor), root);

        return RE::BSEventNotifyControl::kContinue;
    }
}
