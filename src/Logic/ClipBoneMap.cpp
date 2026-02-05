#include "ClipBoneMap.h"
#include "Utils/BoneUtils.h"

namespace {
    using SV = std::string_view;

    // ==============================
    // 冻结骨骼定义
    // ==============================

    constexpr SV kUpperBodyBones[] = {"NPC Spine1 [Spn1]", "NPC Spine2 [Spn2]", "NPC Clavicle L [ClvL]","NPC Clavicle R [ClvR]","NPC Neck [Neck]","NPC Head [Head]"};

    constexpr SV kHeadOnlyBones[] = {"NPC Neck [Neck]", "NPC Head [Head]"};

    constexpr SV kFootOnlyBones[] = { "NPC Foot L [FootL]", "NPC Toe0 L [ToeL]","NPC Foot R [FootR]", "NPC Toe0 R [ToeR]"};

    constexpr SV kLeftLegBones[] = {"NPC L Thigh [LThg]", "NPC L Calf [LClf]", "NPC L Foot [Lft ]"};

    // ==============================
    // Clip → Bone 映射表
    // ==============================

    constexpr Logic::ClipBoneRule kRules[] = {
        {"IdleForceDefaultState", kUpperBodyBones},
                                             
        {"IdleChair", kUpperBodyBones},
                                             
        {"attackPowerStart", kHeadOnlyBones},
                                             
        {"IdleFootLock", kFootOnlyBones},
                                              
        {"mt_walkforward", kLeftLegBones}
    };
}

namespace Logic {
    std::span<const ClipBoneRule> GetClipBoneRules() noexcept { return kRules; }
}
