#pragma once

#include <cstdint>

#include "RE/H/hkQsTransform.h"

namespace RE {
    class BShkbAnimationGraph;
}

namespace HavokPose {
    /**
     * @brief 获取当前帧的 Havok local pose transforms
     *
     * @param a_output              hkbGeneratorOutput（来自 animation graph）
     * @param a_expectedBoneCount   骨骼数量（通常为 graph->numAnimBones）
     * @return hkQsTransform*       local transforms，失败返回 nullptr
     */
    [[nodiscard]]
    RE::hkQsTransform* GetLocalPose(RE::hkbGeneratorOutput* a_output, std::uint16_t a_expectedBoneCount);
}  // namespace HavokPose