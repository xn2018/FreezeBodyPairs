#pragma once

#include <unordered_set>
#include <mutex>

#include "RE/Skyrim.h"

namespace Hook
{
    class ActorLifecycleManager
    {
    public:
        static ActorLifecycleManager& GetSingleton();

        // 插件初始化时调用
        void Initialize();

        // Freeze ON / OFF
        void OnActorFrozen(const RE::Actor* actor);
        void OnActorUnfrozen(const RE::Actor* actor);

        // Actor 生命周期终点
        void OnActorDestroyed(const RE::Actor* actor);

        // AnimationGraph 回调
        void OnActor3DLoaded(const RE::Actor* actor);

    private:
        void RegisterAnimationGraph(const RE::Actor* actor);
        void UnregisterAnimationGraph(const RE::Actor* actor);

        std::unordered_set<const RE::Actor*> _tracked;
        std::mutex _lock;
    };
}
