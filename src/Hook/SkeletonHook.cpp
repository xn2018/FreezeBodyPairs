#include "SkeletonHook.h"

#include <unordered_map>
#include <unordered_set>
#include <mutex>

#include "REL/Relocation.h"
#include "SKSE/Trampoline.h"
#include "RE/N/NiNode.h"
#include "RE/N/NiAVObject.h"
#include "RE/A/Actor.h"

#include "RE/H/hkVector4.h"

#include "Config.h"
#include "Utils/BoneUtils.h"
#include "Utils/FreezeBoneManager.h"
#include "Utils/HavokPose.h"

namespace Hook
{
    using NiAVObject = RE::NiAVObject;
    using NiNode = RE::NiNode;

    static std::unordered_map<RE::FormID, bool> _actorFrozenMap;
    static std::shared_mutex _mutex;

    static inline REL::Relocation<decltype(hkbClipGenerator_Generate)> _hkbClipGenerator_Generate;

    static RE::BShkbAnimationGraph* GetGraphFromGenerator(RE::hkbClipGenerator* gen) {
        if (!gen || gen->userData == 0) {
            return nullptr;
        }

        return reinterpret_cast<RE::BShkbAnimationGraph*>(gen->userData);
    }

    // ===== detour =====
    void hkbClipGenerator_Generate(
        RE::hkbClipGenerator* gen, 
        const RE::hkbContext& ctx,             
        const RE::hkbGeneratorOutput** activeChildrenOutput,                     
        RE::hkbGeneratorOutput& out, 
        float a_timeOffset){

        _hkbClipGenerator_Generate(gen, ctx, activeChildrenOutput, out, a_timeOffset);
        // if the animation is not fully loaded yet, call generate with our fake clip generator containing the previous
        // animation instead - this avoids seeing the reference pose for a frame
        if (gen->userData != 0xC) {
            return _hkbClipGenerator_Generate(gen, ctx, activeChildrenOutput, out, a_timeOffset);
        }

        // 1. 从 hkbNode(userData) 取回 BS 动画图
        auto* graph = GetGraphFromGenerator(gen);
        if (!graph) {
            return _hkbClipGenerator_Generate(gen, ctx, activeChildrenOutput, out, a_timeOffset);
        }

        RE::Actor* player = RE::PlayerCharacter::GetSingleton();

        if (!IsFrozen(player)) {
            return _hkbClipGenerator_Generate(gen, ctx, activeChildrenOutput, out, a_timeOffset);
        }

        // 2. 缓存骨骼旋转
        auto* mgr = Utils::FreezeBoneManager::GetSingleton();
        mgr->EnsureCache(graph);

        // 冻结阶段（示意）
        const std::uint16_t boneCount = graph->numAnimBones;
        for (std::uint16_t i = 0; i < boneCount; ++i) {
            if (mgr->IsFrozen(graph, i)) {
                auto pose = HavokPose::GetLocalPose(&out, boneCount);
                // 验证 pose 
                logger::warn("FreezeBone: bone '{}' not found", pose->rotation.vec.Length3());
            }
        }
    }


    // ===== 安装 Hook =====
    void Install() {
        // vfunc index = 17
        REL::Relocation<uintptr_t> hkbClipGeneratorVtbl{RE::VTABLE_hkbClipGenerator[0]};
        _hkbClipGenerator_Generate = hkbClipGeneratorVtbl.write_vfunc(0x17, hkbClipGenerator_Generate);
    }

    void SetFreezeActor(const RE::Actor* actor, bool frozen) { 
        if (!actor) {
            return;
        }

        const auto formID = actor->GetFormID();

        std::unique_lock lock(_mutex);

        if (frozen) {
            _actorFrozenMap[formID] = true;
        } else {
            _actorFrozenMap.erase(formID);
        }
    }

    static bool IsFrozen(RE::Actor* actor) {
        if (!actor) {
            return false;
        }

        const auto formID = actor->GetFormID();

        std::shared_lock lock(_mutex);
        return _actorFrozenMap.contains(formID);
    }
}
