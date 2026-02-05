SKSE 插件生命周期 · API 使用对照表

**|阶段 0：DLL 被加载（DllMain）|**

时机：Windows Loader
推荐做的事情：几乎没有

| API / 行为           | 状态       | 说明                  |
| -------------------- | ---------- | --------------------- |
| 任何 SKSE / RE:: API | ❌ 禁止     | Loader Lock，极易死锁 |
| 文件 IO              | ⚠️ 极不推荐 | 可能阻塞加载          |
| spdlog / std::format | ❌ 不安全   | CRT 可能未完成初始化  |
| 创建线程             | ❌ 禁止     | 官方明确反对          |
| Hook / Patch         | ❌ 禁止     | 引擎尚未加载          |


结论：
DllMain 中什么都不要做。

**|阶段 1：SKSEPluginLoad(const LoadInterface) |**

时机：SKSE 正在加载插件，Skyrim 尚未完成初始化
这是你现在遇到问题的阶段

✅ 允许（安全）

| API / 行为                 | 说明         |
| -------------------------- | ------------ |
| `InitializeLogging()`      | 文件日志安全 |
| `log::info / warn / error` | 仅限文件     |
| `SKSE::Init()`             | 必须         |
| 读取 INI / JSON            | 仅限插件配置 |
| 注册 Messaging Listener    | 推荐         |
| 注册 Papyrus 函数          | 可行         |

❌ 禁止（会“吃掉输出”或崩）

| API / 行为              | 原因               |
| ----------------------- | ------------------ |
| `RE::ConsoleLog::Print` | Console 尚未初始化 |
| `RE::DebugNotification` | UI 未就绪          |
| `RE::PlayerCharacter`   | 世界未创建         |
| `RE::TESDataHandler`    | Form 尚未加载      |
| `RE::UI`                | UI 系统未初始化    |
| Hook 引擎函数           | 目标代码尚未加载   |


时机：即将加载存档 / 新游戏
状态：数据仍不完整

| API / 行为     | 状态     | 说明          |
| -------------- | -------- | ------------- |
| 日志           | ✅        | 文件日志      |
| 控制台输出     | ⚠️ 有风险 | 不保证 UI     |
| Player / World | ❌        | 尚未创建      |
| 表单访问       | ❌        | Form 表不完整 |


**|阶段 3：SKSE Messaging · kDataLoaded（最重要）|**

时机：

所有 ESM / ESP / ESL 加载完成

世界和控制台已初始化

插件初始化的黄金阶段

✅ 推荐在这里做的事情

| API / 行为              | 状态                |
| ----------------------- | ------------------- |
| `RE::ConsoleLog::Print` | ✅ 完全安全          |
| `RE::DebugNotification` | ✅                   |
| `RE::TESDataHandler`    | ✅                   |
| `RE::PlayerCharacter`   | ⚠️（首次访问可延迟） |
| 安装 Hook               | ✅                   |
| 注册事件监听            | ✅                   |
| 初始化全局状态          | ✅                   |


绝大多数插件的“真正初始化”都应放在这里

**|阶段 4：SKSE Messaging · kPostLoadGame|**

时机：存档加载完成，玩家已进入世界

| API / 行为              | 状态                |
| ----------------------- | ------------------- |
| `RE::ConsoleLog::Print` | ✅ 完全安全          |
| `RE::DebugNotification` | ✅                   |
| `RE::TESDataHandler`    | ✅                   |
| `RE::PlayerCharacter`   | ⚠️（首次访问可延迟） |
| 安装 Hook               | ✅                   |
| 注册事件监听            | ✅                   |
| 初始化全局状态          | ✅                   |



**|阶段 5：运行期（游戏进行中）|**

时机：正常游戏运行

| API / 行为 | 状态          |
| ---------- | ------------- |
| ConsoleLog | ✅             |
| UI 操作    | ✅             |
| Hook 回调  | ✅             |
| 多线程     | ⚠️（必须同步） |
| 文件 IO    | ⚠️（避免频繁） |

**|阶段 6：SKSE Messaging · kExitGame|**
时机：退出游戏 / 返回主菜单

| API / 行为 | 状态 | 说明            |
| ---------- | ---- | --------------- |
| 日志       | ✅    |                 |
| Console    | ⚠️    | UI 可能正在销毁 |
| 保存状态   | ⚠️    | 快速完成        |
| 卸载 Hook  | ⚠️    | 通常不需要      |


**|阶段 7：DLL 卸载|**
几乎不会发生（Skyrim 常驻）

| API / 行为 | 状态 |
| ---------- | ---- |
| RE:: API   | ❌    |
| 线程 join  | ❌    |
| Hook 清理  | ❌    |

一句话速记版（给你记忆用）

Load：只做日志 + 注册

DataLoaded：做一切
PostLoadGame：碰玩家

永远不要在 Load 里碰 UI / Console / Player