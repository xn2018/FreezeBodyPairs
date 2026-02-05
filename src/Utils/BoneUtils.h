#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>

#include "RE/A/Actor.h"
#include "RE/N/NiNode.h"

namespace Utils::BoneUtils {
    // 1. 解析角色第三人称骨架根节点
    RE::NiNode* GetSkeletonRoot(const RE::Actor* actor) noexcept;
    // 返回“空骨骼列表”（语义：不冻结任何骨骼）
    
    // 2. 在骨架中按名称查找骨骼
    RE::NiAVObject* FindBone(RE::NiNode* skeletonRoot, std::string_view boneName) noexcept;

    struct FrozenBoneState {
        RE::NiAVObject* bone;
        RE::stl::enumeration<RE::NiAVObject::Flag, std::uint32_t> originalFlags;
    };

    static std::unordered_map<RE::NiAVObject*, FrozenBoneState> _frozenBones;
    static std::mutex _freezeLock;

    // Store original bone transforms for foot bones
    class BoneTransformManager {
    public:
        static BoneTransformManager* GetSingleton() {
            static BoneTransformManager singleton;
            return &singleton;
        }

        void CaptureFootBones(RE::PlayerCharacter* a_player);
        void RestoreFootBones(RE::PlayerCharacter* a_player);
        bool HasCapturedBones() const { return m_hasCapturedBones; }
        void Clear();

    private:
        BoneTransformManager() = default;

        struct BoneTransform {
            RE::NiPoint3 position;
            RE::NiMatrix3 rotation;
            float scale;
        };

        std::unordered_map<std::string, BoneTransform> m_footBoneTransforms;
        bool m_hasCapturedBones = false;

        // Foot bone names (commonly used in Skyrim skeletons)
        const std::vector<std::string> m_footBoneNames = {"NPC L Foot [Lft ]", "NPC R Foot [Rft ]","NPC L Toe0 [LToe]", "NPC R Toe0 [RToe]",};
    };
}