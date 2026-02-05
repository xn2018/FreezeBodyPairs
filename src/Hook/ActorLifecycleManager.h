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

        // Actor 的 3D / Skeleton 已经可用
        static void OnActor3DLoaded(const RE::Actor* actor);

        // 可选：Actor 即将卸载 3D
        static void OnActor3DUnloaded(const RE::Actor* actor);

    private:
        void RegisterAnimationGraph(const RE::Actor* actor);
        void UnregisterAnimationGraph(const RE::Actor* actor);

        std::unordered_set<RE::FormID> _tracked;
        std::mutex _lock;
    };
}
