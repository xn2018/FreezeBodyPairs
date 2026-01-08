#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

namespace Hook {
    static std::mutex g_animMutex;
    class ActorAnimationEventSink : public RE::BSTEventSink<RE::BSAnimationGraphEvent> {
    public:
        static ActorAnimationEventSink& GetSingleton() noexcept;
        std::unordered_set<const RE::Actor*> _registered;
        // 注册 sink 到指定 actor（在 Actor 的 AnimationGraph 初始化完成后调用）
        void RegisterForActor(const RE::Actor* actor) noexcept;
        void Unregister(const RE::Actor* actor) noexcept;

        virtual RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* evn,
            RE::BSTEventSource<RE::BSAnimationGraphEvent>* src) override;
    };
}