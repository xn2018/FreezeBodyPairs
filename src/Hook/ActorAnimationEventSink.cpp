#include "Actor3DIndex.h"
#include "ActorAnimationEventSink.h"
#include "Utils/BoneUtils.h"
#include "Utils/FreezeManager.h"
#include "Hook/ActorLifecycleManager.h"

#include <stack>
#include <fstream>

using namespace SKSE::log;

namespace Hook {
    ActorAnimationEventSink& ActorAnimationEventSink::GetSingleton() noexcept {
        static ActorAnimationEventSink inst;
        return inst;
    }

    void ActorAnimationEventSink::RegisterForActor(const RE::Actor* actor) noexcept {
        if (!actor) {
            return;
        }

        // 1. 获取 AnimationGraphManager（正确接口）
        RE::BSTSmartPointer<RE::BSAnimationGraphManager> mgr;
        if (!actor->GetAnimationGraphManager(mgr)) {
            return;
        }

        if (!mgr) {
            return;
        }

        // 2. 获取主 Graph（通常 index 0）
        if (mgr->graphs.empty()) {
            return;
        }

        auto* graph = mgr->graphs[0].get();
        // 3. 获取事件源
        auto* source = graph->GetEventSource<RE::BSAnimationGraphEvent>();
        if (!source) {
            return;
        }

        source->AddEventSink(&ActorAnimationEventSink::GetSingleton());

        logger::info("Registered BSAnimationGraphEvent sink for actor {}", (void*)actor);
    }

    void ActorAnimationEventSink::Unregister(const RE::Actor* actor) noexcept {
        if (!actor) {
            return;
        }

        // ⚠ 注意：这里 actor 仍然活着，但 graph 可能已经销毁
        RE::BSTSmartPointer<RE::BSAnimationGraphManager> mgr;
        if (!actor->GetAnimationGraphManager(mgr)) {
            return;
        }

        if (!mgr) {
            return;
        }

        // 非零说明 graphs 有值
        if (mgr->graphs.empty()) {
            return;
        }

        auto* graph = mgr->graphs[0].get();
        auto* source = graph->GetEventSource<RE::BSAnimationGraphEvent>();
        if (!source) {
            return;
        }

        source->RemoveEventSink(this);

        std::lock_guard<std::mutex> lk(g_animMutex);
        _registered.erase(actor);
    }

    static void HandleActor3DDestroyed(RE::Actor* actor) {
        if (!actor) {
            return;
        }

        // 1. 解除所有冻结（最重要）
        Utils::FreezeManager::GetSingleton().On3DUnloaded(actor);

        // 2. 清理 3D 反向索引
        Hook::Actor3DIndex::GetSingleton().Unregister(actor);

        // 3. 生命周期管理（可扩展）
        Hook::ActorLifecycleManager::GetSingleton().OnActor3DUnloaded(actor);

        logger::info("Actor3D destroyed: actor={}", (void*)actor);
    }

    RE::BSEventNotifyControl ActorAnimationEventSink::ProcessEvent(const RE::BSAnimationGraphEvent* evn,
                                                                   RE::BSTEventSource<RE::BSAnimationGraphEvent>*) {
        if (!evn) {
            return RE::BSEventNotifyControl::kContinue;
        }

        auto actor = evn->holder ? evn->holder->As<RE::Actor>() : nullptr;
        if (!actor) {
            return RE::BSEventNotifyControl::kContinue;
        }

        auto _actor = const_cast<RE::Actor*>(actor);
        const auto& tag = evn->tag;

        switch (hash::fnv1a_32(tag.data()))  // 或你已有的字符串 hash 工具
        {
            // =====================================================
            // 3D 加载完成
            // =====================================================
            case hash::fnv1a_32("On3DLoaded"): {
                if (auto* root = actor->Get3D()) {
                    Hook::Actor3DIndex::GetSingleton().Register(_actor, root);
                }

                Hook::ActorLifecycleManager::GetSingleton().OnActor3DLoaded(actor);
                break;
            }

            // =====================================================
            // 3D / Graph 销毁事件（全部集中在这里）
            // =====================================================
            case hash::fnv1a_32("On3DUnloaded"):
            case hash::fnv1a_32("OnGraphDestructed"):
            case hash::fnv1a_32("OnGraphDestroyed"):  // 某些版本存在该名字
            case hash::fnv1a_32("OnUnload"):          // 少数动画节点会发这个
            {
                HandleActor3DDestroyed(_actor);
                break;
            }

            // =====================================================
            // 其他事件（暂时忽略）
            // =====================================================
            default:
                break;
        }

        return RE::BSEventNotifyControl::kContinue;
    }
}
