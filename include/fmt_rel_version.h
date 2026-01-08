#pragma once

#include <fmt/core.h>

#include "REL/Relocation.h"

// 特化 fmt::formatter<REL::Version>
// 解决 fmt fallback formatter 对 const this 的不兼容问题（C2662）
template <>
struct fmt::formatter<REL::Version> {
    // 无格式控制，跳过
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    // 正式格式化函数（必须为 const）
    template <typename FormatContext>
    auto format(const REL::Version& ver, FormatContext& ctx) const {
        // 使用 REL::Version 自带的安全输出方法
        return fmt::format_to(ctx.out(), "{}", ver.string());
    }
};
