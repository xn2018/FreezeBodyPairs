#include "Utils/HavokPose.h"

#include "REL/Relocation.h"

namespace {
    /**
     * @brief SE / AE 下 hkbGeneratorOutput 中 pose 指针的偏移
     */
    [[nodiscard]]
    std::size_t GetPoseOffset() {
        if (REL::Module::GetRuntime() >= REL::Module::Runtime::AE) {
            return 0x30;  // AE 1.6.x
        }
        return 0x20;  // SE 1.5.97
    }
}  // namespace

namespace HavokPose {
    RE::hkQsTransform* GetLocalPose(RE::hkbGeneratorOutput* a_output, std::uint16_t a_expectedBoneCount) {
        if (!a_output || a_expectedBoneCount == 0) return nullptr;

        // Skyrim 最大动画骨骼数远小于 512
        if (a_expectedBoneCount > 512) return nullptr;

        const auto base = reinterpret_cast<std::uintptr_t>(a_output);
        const auto poseAddr = base + GetPoseOffset();

        auto* pose = *reinterpret_cast<RE::hkQsTransform**>(poseAddr);

        if (!pose) return nullptr;

        return pose;
    }
}  // namespace HavokPose
