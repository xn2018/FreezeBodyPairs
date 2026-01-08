#include "Actor3DIndex.h"

using namespace SKSE::log;

namespace Hook {
    Actor3DIndex& Actor3DIndex::GetSingleton() noexcept {
        static Actor3DIndex inst;
        return inst;
    }

    void Actor3DIndex::Register(RE::Actor* actor, RE::NiAVObject* root) noexcept {
        if (!actor || !root) return;

        std::lock_guard lk(_lock);

        // 如果旧 root 存在，替换索引
        auto old = _actorToRoot.find(actor);
        if (old != _actorToRoot.end()) {
            _rootToActor.erase(old->second);
        }

        _actorToRoot[actor] = root;
        _rootToActor[root] = actor;

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

    RE::Actor* Actor3DIndex::GetActorByRoot(RE::NiAVObject* root) noexcept {
        if (!root) return nullptr;

        std::lock_guard lk(_lock);

        auto it = _rootToActor.find(root);
        if (it != _rootToActor.end()) {
            return it->second;
        }
        return nullptr;
    }

    void Actor3DIndex::Clear() noexcept {
        std::lock_guard lk(_lock);
        _rootToActor.clear();
        _actorToRoot.clear();
    }
}