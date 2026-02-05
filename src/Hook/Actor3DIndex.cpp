#include "Actor3DIndex.h"
#include "Hook/ActorLifecycleManager.h"

using namespace SKSE::log;

namespace Hook {
    Actor3DIndex& Actor3DIndex::GetSingleton() noexcept {
        static Actor3DIndex inst;
        return inst;
    }

    void Actor3DIndex::Register(RE::Actor* actor, RE::NiAVObject* root) noexcept 
    {

        if (!actor || !root) {
            return;
        }

        std::lock_guard lk(_lock);

        // 如果旧 root 存在，清理双向索引
        auto old = _actorToRoot.find(actor);
        if (old != _actorToRoot.end()) {
            auto* oldRoot = old->second;
            _rootToActor.erase(oldRoot);
            _actorToRoot.erase(old);
        }

        _actorToRoot.emplace(actor, root);
        _rootToActor.emplace(root, actor);

        logger::info("Registered Actor3D: actor={} root={}", (void*)actor, (void*)root);
    }

    void Actor3DIndex::Unregister(RE::Actor* actor) noexcept {
        if (!actor) return;

        std::lock_guard lk(_lock);

        auto it = _actorToRoot.find(actor);
        if (it != _actorToRoot.end()) {
            auto root = it->second;
            _rootToActor.erase(root);
            _actorToRoot.erase(it);

            logger::info("Unregistered Actor3D: actor={} root={}", (void*)actor, (void*)root);
        }
    }

    RE::Actor* Actor3DIndex::GetActorByRoot(RE::NiAVObject* root) noexcept 
    {
        if (!root) {
            return nullptr;
        }

        std::lock_guard lk(_lock);

        auto it = _rootToActor.find(root);
        if (it == _rootToActor.end()) {
            return nullptr;
        }

        auto* actor = it->second;
        if (!actor) {
            return nullptr;
        }

        // 非常关键：最低限度生命周期校验
        if (actor->IsDeleted() || actor->IsDead() || actor->IsDisabled()) {
            return nullptr;
        }

        return actor;
    }

    void Actor3DIndex::Clear() noexcept {
        std::lock_guard lk(_lock);
        _rootToActor.clear();
        _actorToRoot.clear();
    }

    void Actor3DIndex::On3DLoaded(const RE::Actor* actor) {
        Hook::ActorLifecycleManager::OnActor3DLoaded(actor);
    }
}