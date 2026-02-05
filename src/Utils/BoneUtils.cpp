#include "Utils/BoneUtils.h"
#include "Logic/ClipBoneMap.h"
#include "RE/N/NiTransform.h"

using namespace SKSE;
using namespace SKSE::log;

namespace Utils::BoneUtils {
    // ------------------------------------------------------------
    // Skeleton
    // ------------------------------------------------------------

    RE::NiNode* GetSkeletonRoot(const RE::Actor* actor) noexcept {
        if (!actor) {
            return nullptr;
        }

        const auto& biped = actor->GetBiped();
        if (!biped) {
            return nullptr;
        }

        // NG 中第三人称骨架的唯一稳定入口
        auto* root = biped->root;
        return root ? root->AsNode() : nullptr;
    }
     
    // ------------------------------------------------------------
    // Bone lookup
    // ------------------------------------------------------------

    RE::NiAVObject* FindBone(RE::NiNode* skeletonRoot, std::string_view boneName) noexcept {
        if (!skeletonRoot || boneName.empty()) {
            return nullptr;
        }

        // 注意：GetObjectByName 使用 BSFixedString
        return skeletonRoot->GetObjectByName(RE::BSFixedString(boneName.data()));
    }

    void BoneTransformManager::CaptureFootBones(RE::PlayerCharacter* a_player) {
        if (!a_player || !a_player->Get3D()) {
            log::warn("Cannot capture foot bones - player or 3D model is null");
            return;
        }

        m_footBoneTransforms.clear();
        auto root = a_player->Get3D();

        for (const auto& boneName : m_footBoneNames) {
            auto bone = root->GetObjectByName(boneName);
            if (bone) {
                BoneTransform transform;
                transform.position = bone->local.translate;
                transform.rotation = bone->local.rotate;
                transform.scale = bone->local.scale;

                m_footBoneTransforms[boneName] = transform;
                log::trace("Captured bone: {} at pos({}, {}, {})", boneName, transform.position.x,
                              transform.position.y, transform.position.z);
            } else {
                log::warn("Could not find bone: {}", boneName);
            }
        }

        m_hasCapturedBones = !m_footBoneTransforms.empty();
        log::info("Captured {} foot bones", m_footBoneTransforms.size());

        if (!m_hasCapturedBones) {
            log::error("Failed to capture any foot bones! Check if bone names match your skeleton.");
        }
    }

    void BoneTransformManager::RestoreFootBones(RE::PlayerCharacter* a_player) {
        if (!a_player || !a_player->Get3D() || m_footBoneTransforms.empty()) {
            return;
        }

        auto root = a_player->Get3D();

        for (const auto& [boneName, transform] : m_footBoneTransforms) {
            auto bone = root->GetObjectByName(boneName);
            if (bone) {
                // Restore the captured transform
                bone->local.translate = transform.position;
                bone->local.rotate = transform.rotation;
                bone->local.scale = transform.scale;
            }
        }
    }

    void BoneTransformManager::Clear() {
        m_footBoneTransforms.clear();
        m_hasCapturedBones = false;
    }
}
