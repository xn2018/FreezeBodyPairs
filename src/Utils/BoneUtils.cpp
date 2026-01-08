// Utils/BoneUtils.cpp
#include "BoneUtils.h"

#include <unordered_map>
#include <stack>
#include <fstream>
#include "nlohmann/json.hpp"

#include "Input/InputHook.h"

namespace Utils::BoneUtils
{
    using json = nlohmann::json;
    // 深度遍历 skeleton，构建 name → NiNode*
    static void BuildNameMap(RE::NiNode* root, std::unordered_map<std::string, RE::NiNode*>& out)
    {
        if (!root)
            return;

        std::stack<RE::NiNode*> st;
        st.push(root);

        while (!st.empty()) {
            auto* n = st.top();
            st.pop();

            if (n->name.c_str()) {
                out.emplace(n->name.c_str(), n);
            }

            for (auto& c : n->GetChildren()) {
                if (auto* cn = c ? c->AsNode() : nullptr) {
                    st.push(cn);
                }
            }
        }
    }

    std::optional<std::uint16_t> FindBoneIndexByName(RE::BShkbAnimationGraph* graph, std::string_view boneName) {
        if (!graph || boneName.empty()) {
            return std::nullopt;
        }

        const std::uint16_t count = graph->numAnimBones;

        for (std::uint16_t i = 0; i < count; ++i) {
            const auto& entry = graph->boneNodes[i];
            if (!entry.node) {
                continue;
            }

            const auto& name = entry.node->name;
            if (name.empty()) {
                continue;
            }

            // case-insensitive compare
            if (_stricmp(name.c_str(), boneName.data()) == 0) {
                return i;
            }
        }

        return std::nullopt;
    }

    static void CollectChildBones(RE::BShkbAnimationGraph* graph, std::uint16_t root,
                                  std::unordered_set<std::uint16_t>& out) {
        if (!out.insert(root).second) return;

        const std::uint16_t count = graph->numAnimBones;

        for (std::uint16_t i = 0; i < count; ++i) {
            if (graph->boneNodes[i].node->parentIndex == root) {
                CollectChildBones(graph, i, out);
            }
        }
    }

    // ===== Actor 查找（示例实现，按你工程已有逻辑替换）=====
    RE::Actor* GetActorFromNode(RE::NiNode* node)
    {
        // 常见做法：向上找 skeleton root 的 user data / owner
        // 这里给一个安全占位实现（假定你已有稳定实现）
        auto* root = node;
        while (root && root->parent) {
            root = root->parent->AsNode();
        }
        // TODO: 根据你工程中既有方式返回 Actor
        return nullptr;
    }

    void ClearCachedTransforms(const RE::Actor* actor) {

    }

    void RebuildFrozenBones(const RE::Actor* actor) {

    }
}
