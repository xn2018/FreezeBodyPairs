#include "Utils/FreezeManager.h"
#include "Logic//ClipBoneMap.h"
#include "Utils/BoneUtils.h"

using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace Utils {
    FreezeManager& FreezeManager::GetSingleton() noexcept {
        static FreezeManager inst;
        return inst;
    }

    void FreezeManager::QueueFreezeRequest(RE::Actor* actor, RE::NiAVObject* bone) noexcept {
        if (!actor || !bone) {
            return;
        }

        {
            std::lock_guard lk(_queueLock);
            _pending.push_back({actor, bone});
        }

        // ★关键：立即投递到主线程处理
        SKSE::GetTaskInterface()->AddTask([] { FreezeManager::GetSingleton().ProcessQueue(); });
    }

    void FreezeManager::QueueFreezeRequest(RE::Actor* actor, std::span<RE::NiAVObject*> bones) noexcept {
        if (!actor || bones.empty()) {
            return;
        }

        {
            std::lock_guard lk(_queueLock);
            for (auto* bone : bones) {
                if (bone) {
                    _pending.push_back({actor, bone});
                }
            }
        }

        SKSE::GetTaskInterface()->AddTask([] { FreezeManager::GetSingleton().ProcessQueue(); });
    }

    void FreezeManager::QueueFreezeByRule(RE::Actor* actor, RE::NiNode* skeletonRoot, std::string_view clipName) noexcept {
        if (!actor || !skeletonRoot || clipName.empty()) {
            return;
        }

        // 查规则（纯 CPU 操作，线程安全）
        const auto& rules = Logic::GetClipBoneRules();

        const Logic::ClipBoneRule* matched = nullptr;

        for (const auto& rule : rules) {
            if (rule.clipName == clipName) {
                matched = std::addressof(rule);
                break;
            }
        }

        if (!matched) {
            return;  // 当前 clip 无冻结规则
        }

        // 在动画线程中解析 bone 指针（只查树，不改数据，安全）
        std::vector<RE::NiAVObject*> bones;
        bones.reserve(matched->bones.size());

        for (auto name : matched->bones) {
            if (auto* bone = Utils::BoneUtils::FindBone(skeletonRoot, name)) {
                bones.push_back(bone);
            }
        }

        if (bones.empty()) {
            return;
        }

        
        // 批量投递冻结任务（进入主线程执行）
        QueueFreezeRequest(actor, bones);

    }


    void FreezeManager::ProcessQueue() noexcept {
        std::vector<FreezeRequest> local;

        {
            std::lock_guard lk(_queueLock);
            local.swap(_pending);
        }

        if (local.empty()) {
            return;
        }

        for (auto& req : local) {
            FreezeBoneInternal(req.actor, req.bone);
        }
    }

    void FreezeManager::FreezeBoneInternal(RE::Actor* actor, RE::NiAVObject* bone) noexcept {
        log::info("进入主线程 FreezeBoneInternal");

        if (!actor || !bone) {
            return;
        }

        if (actor->IsDeleted() || actor->IsDisabled() || actor->IsDead()) {
            return;
        }

        auto* root = actor->Get3D();
        if (!root) {
            return;
        }

        if (!bone->parent) {
            return;
        }

        std::lock_guard lk(_dataLock);

        auto& list = _actorFrozen[actor];

        for (auto& st : list) {
            if (st.bone == bone) {
                return;
            }
        }

        FrozenBoneState state;
        state.bone = bone;
        state.originalFlags = bone->GetFlags();

        // ★ 关键：记录当前姿态
        state.originalLocal = bone->local;
        state.originalWorld = bone->world;

        list.push_back(state);

        log::info("骨骼已加入冻结列表: {}", bone->name.c_str());
    }

    void FreezeManager::ApplyFrozenBones(RE::NiUpdateData* a_data) noexcept {
        std::lock_guard lk(_dataLock);

        for (auto& [actor, list] : _actorFrozen) {
            if (!actor || actor->IsDeleted()) {
                continue;
            }

            auto* root = actor->Get3D();
            if (!root) {
                continue;
            }

            for (auto& st : list) {
                auto* bone = st.bone;
                if (!bone || !bone->parent) {
                    continue;
                }

                // ★ 强制锁死 transform（动画刚写完，这里立刻覆盖回去）
                bone->local = st.originalLocal;
                bone->world = st.originalWorld;

                bone->UpdateWorldData(a_data);
            }
        }
    }

    void FreezeManager::UnfreezeActor(RE::Actor* actor) noexcept {
        if (!actor) {
            return;
        }

        // 强制主线程
        SKSE::GetTaskInterface()->AddTask([actor] { FreezeManager::GetSingleton().UnfreezeActorInternal(actor); });
    }


    void FreezeManager::UnfreezeActorInternal(RE::Actor* actor) noexcept {
        std::lock_guard lk(_dataLock);

        auto it = _actorFrozen.find(actor);
        if (it == _actorFrozen.end()) {
            return;
        }

        for (auto& st : it->second) {
            if (st.bone) {
                st.bone->GetFlags() = st.originalFlags;
            }
        }

        _actorFrozen.erase(it);
    }

    void FreezeManager::On3DUnloaded(RE::Actor* actor) noexcept { 
        UnfreezeActor(actor); 
    }

    void FreezeManager::ClearAll() noexcept {
        SKSE::GetTaskInterface()->AddTask([] {
            auto& mgr = FreezeManager::GetSingleton();

            std::lock_guard lk(mgr._dataLock);

            for (auto& [actor, list] : mgr._actorFrozen) {
                for (auto& st : list) {
                    if (st.bone) {
                        st.bone->GetFlags() = st.originalFlags;
                    }
                }
            }

            mgr._actorFrozen.clear();
        });
    }


}