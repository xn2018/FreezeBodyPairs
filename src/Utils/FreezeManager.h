#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>

namespace Utils {

    class FreezeManager {
    public:
        static FreezeManager& GetSingleton() noexcept;

        // 动画线程调用（安全投递）
        void QueueFreezeRequest(RE::Actor* actor, RE::NiAVObject* bone) noexcept;

        // 批量接口（推荐你之后使用）
        void QueueFreezeRequest(RE::Actor* actor, std::span<RE::NiAVObject*> bones) noexcept;

        // Rule 驱动接口（动画线程安全调用）
        void QueueFreezeByRule(RE::Actor* actor, RE::NiNode* skeletonRoot, std::string_view clipName) noexcept;

        // 主线程执行（由 TaskInterface 自动调用）
        void ProcessQueue() noexcept;

        // 生命周期回调
        void UnfreezeActor(RE::Actor* actor) noexcept;
        void On3DUnloaded(RE::Actor* actor) noexcept;
        void ClearAll() noexcept;

    private:
        struct FrozenBoneState {
            RE::NiAVObject* bone;
            RE::stl::enumeration<RE::NiAVObject::Flag, std::uint32_t> originalFlags;
            RE::NiTransform originalLocal;
            RE::NiTransform originalWorld;
        };

        // Actor -> frozen bones
        std::unordered_map<RE::Actor*, std::vector<FrozenBoneState>> _actorFrozen;

        struct FreezeRequest {
            RE::Actor* actor;
            RE::NiAVObject* bone;
        };

        std::vector<FreezeRequest> _pending;
        std::mutex _queueLock;
        std::mutex _dataLock;

        void FreezeBoneInternal(RE::Actor* actor, RE::NiAVObject* bone) noexcept;
        void ApplyFrozenBones(RE::NiUpdateData* a_data) noexcept;
        void UnfreezeActorInternal(RE::Actor* actor) noexcept;
    };

}
