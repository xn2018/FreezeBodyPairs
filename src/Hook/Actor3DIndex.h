#pragma once

#include <mutex>
#include <unordered_map>

#include "RE/Skyrim.h"

namespace Hook {
    class Actor3DIndex {
    public:
        static Actor3DIndex& GetSingleton() noexcept;

        // 注册 3D：actor —> root
        void Register(RE::Actor* actor, RE::NiAVObject* root) noexcept;

        // 卸载 3D（或 actor 死亡/卸载）
        void Unregister(RE::Actor* actor) noexcept;

        // 根据 skeleton root 查 actor
        RE::Actor* GetActorByRoot(RE::NiAVObject* root) noexcept;

        static void On3DLoaded(const RE::Actor* actor);
        // 清空（插件卸载等）
        void Clear() noexcept;

    private:
        Actor3DIndex() = default;
        ~Actor3DIndex() = default;

        Actor3DIndex(const Actor3DIndex&) = delete;
        Actor3DIndex& operator=(const Actor3DIndex&) = delete;

        mutable std::mutex _lock;

        // root → actor
        std::unordered_map<RE::NiAVObject*, RE::Actor*> _rootToActor;

        // actor → root
        std::unordered_map<RE::Actor*, RE::NiAVObject*> _actorToRoot;
    };
}  // namespace FreezeBodyPairs
