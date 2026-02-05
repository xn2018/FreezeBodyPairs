#pragma once
#include <span>
#include <string_view>

namespace Logic {
    struct ClipBoneRule {
        std::string_view clipName;
        std::span<const std::string_view> bones;
    };

    std::span<const ClipBoneRule> GetClipBoneRules() noexcept;
}
