# AGENTS.md

本文件是给 AI 编码助手使用的仓库工作指南。所有在本仓库中进行的分析、修改、测试和提交说明，都应优先遵循这里的约定。

## 项目定位

本仓库属于 OpenHarmony Bundle Manager Distributed Bundle Framework（分布式包管理服务 DBMS），负责跨设备的应用包信息同步与查询能力，包括获取远程设备 AbilityInfo、跨设备 BundleInfo 查询、分布式数据存储与同步、远程版本号与元数据查询。

工作时请把它视为系统级基础组件：接口稳定性、跨设备权限边界、数据同步一致性、UDID/UUID 隐私保护和错误码兼容性都很重要。

## 工作原则

- 修改前先阅读相关目录、调用链和已有测试，优先沿用现有风格。
- 保持改动范围收敛，不做与任务无关的重构、格式化或命名调整。
- 不回滚用户已有改动；遇到工作区脏文件时，只处理与当前任务直接相关的文件。
- 对跨设备 IPC、分布式数据同步、权限校验、设备标识解析等逻辑保持谨慎。
- 新增行为应尽量有测试或至少说明无法测试的原因。
- 日志、错误码、权限检查和空指针检查应遵循仓库已有模式。
- 设备标识（UDID/UUID/networkId）属于敏感信息，日志中必须使用 `AnonymizeUdid` 脱敏。

## 代码结构

### 目录与分层

```
应用层 (ArkTS/JS/ANI/C++)
  ↓ interfaces/kits/ (JS/ANI 多语言绑定)
interfaces/inner_api/ IPC接口层 (Proxy/Host/Client：IDistributedBms · 14 组 IPC 消息码)
  ↓
services/dbms/ 服务实现层 (SA 402 d-bms 进程)
  ├─ DistributedBms (SystemAbility + Host 实现)
  ├─ DistributedDataStorage (分布式 KV 数据存储与同步)
  ├─ DbmsDeviceManager (设备管理 · UDID/UUID 解析 · ACL 校验)
  ├─ DistributedMonitor (公共事件订阅 · 包安装/卸载/变更/用户切换)
  ├─ AccountManagerHelper (用户与账号信息获取)
  ├─ ImageCompress (图标压缩 · Base64 编码)
  └─ EventReport (HiSysEvent 上报)
```

### 关键目录

- `interfaces/inner_api/include/`：内部接口定义（`IDistributedBms` 接口、`DistributedBmsProxy`、`DistributedBundleMgrClient`、`DistributedBmsAclInfo`、IPC 消息码枚举）。
- `interfaces/inner_api/src/`：Proxy 和 Client 实现。
- `interfaces/kits/js/`：JS 应用接口绑定（`distributebundlemgr` 旧接口 + `distributedBundle` 新接口 + `distributed_helper`）。
- `interfaces/kits/ani/`：ANI 应用接口绑定。
- `services/dbms/include/`：服务层头文件。
- `services/dbms/src/`：服务层实现代码。
- `services/dbms/sa_profile/`：SA 402 配置（`402.json`、`distributedbms.cfg`）。
- `services/dbms/test/`：单元测试和系统测试场景工程。
- `test/fuzztest/`：Fuzz 测试（`distributedbmshost_fuzzer`、`distributeddatastorage_fuzzer`）。
- `dbms.gni`：编译功能开关定义。
- `bundle.json`：组件元信息和依赖声明。

### 查找路径

按任务类型快速定位关键文件：

| 任务类型 | 关键文件 |
|----------|---------|
| 新增/修改 IPC 方法 | `interfaces/inner_api/include/distributed_bundle_ipc_interface_code.h`（枚举）+ `services/dbms/src/distributed_bms_host.cpp`（Host switch 分发）+ `interfaces/inner_api/src/distributed_bms_proxy.cpp`（Proxy 发送） |
| 修改权限/ACL 校验 | `services/dbms/src/distributed_bms.cpp`（`VerifySystemApp`/`VerifyCallingPermission`/`VerifyCallingPermissionOrAclCheck`/`BuildDistributedBmsAclInfo`/`CheckAclData`） |
| 修改分布式数据同步 | `services/dbms/src/distributed_data_storage.cpp`（KV Store 读写 + `SyncAndCompleted`） |
| 修改设备标识解析 | `services/dbms/src/dbms_device_manager.cpp`（`GetUdidByNetworkId`/`GetUuidByNetworkId`/`GetLocalDevice`） |
| 修改公共事件订阅 | `services/dbms/include/distributed_monitor.h`（`OnReceiveEvent`） |
| 修改 SA 配置 | `services/dbms/sa_profile/402.json` + `services/dbms/sa_profile/distributedbms.cfg` |
| 修改编译开关 | `dbms.gni`（功能开关定义）+ `services/dbms/BUILD.gn`（条件编译分支） |
| 修改 JS 应用接口 | `interfaces/kits/js/distributedBundle/`（新接口）或 `interfaces/kits/js/distributebundlemgr/`（旧接口） |
| 修改 ANI 应用接口 | `interfaces/kits/ani/distributed_bundle_manager/` |
| 修改客户端封装 | `interfaces/inner_api/src/distributed_bundle_mgr_client.cpp`（`DistributedBundleMgrClient`） |
| 修改 HiSysEvent 上报 | `services/dbms/src/event_report.cpp` + `services/dbms/include/event_report.h` |
| 修改图标压缩 | `services/dbms/src/image_compress.cpp` + `services/dbms/include/image_compress.h` |
| 编写单元测试 | `services/dbms/test/unittest/`（`dbms_services_kit_test/`、`distributed_bms_host_test/`） |
| 编写 Fuzz 测试 | `test/fuzztest/`（`distributedbmshost_fuzzer/`、`distributeddatastorage_fuzzer/`） |

## 知识路由

本工程无独立 skill 目录，但与 `bundle_framework` 仓共享部分知识体系。命中以下场景时，建议参考 `bundle_framework` 仓的对应 skill 或文档。

### 按场景路由

| 任务场景 | 参考方向 |
|----------|---------|
| 跨设备 IPC 接口新增/修改 | `bms-add-ipc`（参考其 IPC 注册模式） |
| 权限校验、ACL 检查、系统应用校验 | `bms-security-verify` |
| userId 语义、多用户、`AccountManagerHelper` | `bms-user-model` |
| 日志新增/修改/审查、APP_LOG、UDID 脱敏 | `bms-logging` |
| 测试编写、Fuzz 测试、mock 选择 | `bms-testing-patterns` |
| 源码定位、模块职责、调用链 | `bms-navigation` |

### 按路径路由

当变更涉及以下路径时，编辑前必须先阅读对应文件和关联知识：

| 变更路径 | 需先阅读 | 原因 |
|----------|---------|------|
| `services/dbms/src/distributed_data_storage.cpp` | KV Store 同步协议、Key 格式 `udid_bundleName`、`SyncAndCompleted` 调用链、`DistributedBundleInfo` JSON 格式 | 数据同步关键路径，格式变更影响跨设备兼容性 |
| `services/dbms/src/distributed_bms.cpp` | 权限校验链（`VerifySystemApp`/`VerifyCallingPermissionOrAclCheck`）、ACL 构建流程（`BuildDistributedBmsAclInfo`）、`GetDistributedBundleMgr` 跨设备代理获取 | 权限和跨设备调用入口，变更影响安全边界 |
| `services/dbms/src/distributed_bms_host.cpp` | `OnRemoteRequest` switch 分发、`DistributedInterfaceCode` 枚举、`GetParcelableInfos` 批量上限 | IPC 消息分发入口，遗漏会导致 IPC 不完整 |
| `interfaces/inner_api/include/distributed_bundle_ipc_interface_code.h` | Host `OnRemoteRequest` switch、Proxy `SendRequest` | 新增消息码须三处同步（枚举 + Host 分发 + Proxy 发送） |
| `interfaces/inner_api/src/distributed_bms_proxy.cpp` | `DistributedInterfaceCode` 枚举、`CheckElementName` 入参校验、`SendRequest` 序列化格式 | Proxy 序列化须与 Host 反序列化严格对应 |
| `interfaces/inner_api/src/distributed_bundle_mgr_client.cpp` | SA 402 按需加载逻辑、死亡监听 `DeathRecipient`、`LoadDistributedBundleMgrService` | 客户端生命周期管理，影响服务获取可靠性 |
| `services/dbms/src/dbms_device_manager.cpp` | `device_manager` SDK 接口、UDID/UUID 解析链路、`CheckAclData` ACL 校验 | 设备标识解析和 ACL 校验基础 |
| `services/dbms/src/event_report.cpp` | `DBMSEventType` 枚举、`DBMSEventInfo` 结构、HiSysEvent 上报格式 | 诊断事件格式变更影响故障分析 |
| `services/dbms/include/distributed_monitor.h` | 公共事件 action 列表（`PACKAGE_ADDED`/`PACKAGE_REMOVED`/`PACKAGE_CHANGED`/`USER_SWITCHED`）、`DistributedDataStorage` 调用 | 事件订阅与数据同步触发逻辑 |
| `dbms.gni` | 所有条件编译分支（`BUILD.gn` 中的 `if` 块）、`global_parts_info` 依赖检查 | 开关变更影响构建范围和功能可用性 |
| `services/dbms/sa_profile/402.json` | SA 按需启动策略（`deviceonline`、`longtimeunused-unload`）、`distributedbms.cfg` 进程配置 | SA 生命周期配置变更影响服务可用性 |
| `interfaces/inner_api/include/distributed_bms_acl_info.h` | `DistributedBmsAclInfo` 字段、`Marshalling`/`Unmarshalling` 序列化、`BuildDistributedBmsAclInfo` 构建逻辑 | ACL 信息格式变更影响跨设备校验 |
| `services/dbms/src/image_compress.cpp` | `DISTRIBUTED_BUNDLE_IMAGE_ENABLE` 开关、`ImageCompress` 类接口、Base64 编码逻辑 | 图像处理条件编译，变更影响图标传输 |

### 领域词汇路由

当任务描述、issue、日志、API 名称或变更文件涉及以下术语时，应先了解其语义再规划：

| 术语 | 风险提示 | 说明 |
|------|---------|------|
| SA 402 / d-bms | 分布式包管理服务进程，SAID 为 402 | 进程名为 `d-bms`，按需启动（设备上线启动，长时间空闲 180s 卸载） |
| IDistributedBms | IPC 接口基类，所有分布式包管理能力的入口 | 描述符 `ohos.appexecfwk.IDistributedbms` |
| DistributedInterfaceCode | IPC 消息码枚举，共 14 个 | 新增消息码必须在枚举和 Host `OnRemoteRequest` switch 分发中同步注册 |
| DistributedBmsProxy | IPC 客户端代理，应用层通过 `DistributedBundleMgrClient` 获取 | |
| DistributedBundleMgrClient | 面向内部模块的客户端封装（`DelayedSingleton`） | 负责按需加载 SA 402 并管理死亡监听 |
| DistributedDataStorage | 分布式 KV 数据存储（`DistributedKvDataManager`） | AppId=`bundle_manager_service`，StoreId=`distribute_bundle_datas`，SecurityLevel=S1，EL1 |
| DbmsDeviceManager | 设备管理器封装，基于 `device_manager` | 提供 `GetUdidByNetworkId`/`GetUuidByNetworkId`/`GetLocalDevice`/`CheckAclData` |
| DistributedBmsAclInfo | 跨设备 ACL 校验信息（networkId/userId/accountId/tokenId/pkgName） | 远程调用时由调用方构建并传递，接收方校验 |
| DistributedMonitor | 公共事件订阅器 | 监听 `PACKAGE_ADDED`/`PACKAGE_REMOVED`/`PACKAGE_CHANGED`/`USER_SWITCHED`，触发数据同步 |
| UDID / UUID / networkId | 设备标识符，属于敏感信息 | 日志中必须使用 `AnonymizeUdid` 脱敏，不得明文打印 |
| HICOLLIE_ENABLE | 超时看门狗开关 | 远程调用设置 10s 超时，本地调用设置 5s 超时 |
| HISYSEVENT_ENABLE | HiSysEvent 上报开关 | 通过 `dbms.gni` 中 `hisysevent_enable_dbms` 控制 |
| ACCOUNT_ENABLE | 账号信息获取开关 | 通过 `dbms.gni` 中 `account_enable_dbms` 控制 |
| DISTRIBUTED_BUNDLE_IMAGE_ENABLE | 图像处理开关 | 通过 `dbms.gni` 中 `distributed_bundle_image_framework_enable` 控制 |
| dbms.gni | 编译功能开关集合 | 修改开关默认值会影响构建范围，需确认所有条件编译分支 |
| appexecfwk_errors | 错误码属于公共兼容性边界 | 新增/修改需归类到已有模块偏移 |
| GET_REMOTE_ABILITY_INFO_MAX_SIZE | 批量查询上限为 10 | Host 层 `GetParcelableInfos` 校验，超限直接返回失败 |

### 规划声明

在开始编辑前，必须明确：
- 任务属于哪类场景（跨设备查询/IPC/权限/数据同步/设备管理/日志/测试/...）
- 已读取哪些相关文件或文档
- 发现了哪些约束或边界
- 是否涉及跨设备数据同步或权限校验链

## 约束与边界

### 架构与业务不变量

- **单进程单 SA 架构**：DBMS 仅有一个 SA 402（`d-bms` 进程），不同于 `bundle_framework` 的双进程（SA 401 + SA 511）。所有跨设备调用通过 `samgr->CheckSystemAbility(SA 402, deviceId)` 获取远端 `IDistributedBms` 代理。
- **权限校验分层**：`GetRemoteAbilityInfo`/`GetRemoteAbilityInfos`/`GetRemoteBundleVersionCode`/`GetRemoteMetadata` 须同时校验系统应用（`VerifySystemApp`）和权限（`PERMISSION_GET_BUNDLE_INFO_PRIVILEGED`）；`GetAbilityInfo`/`GetAbilityInfos`/`GetBundleVersionCode`/`GetMetadataByBundleName` 走 ACL 校验或权限校验（`VerifyCallingPermissionOrAclCheck`）；`GetDistributedBundleInfo`/`GetDistributedBundleName` 仅校验权限。
- **跨设备 ACL 校验**：远程调用时，调用方通过 `BuildDistributedBmsAclInfo` 构建 ACL 信息（包含 networkId/userId/accountId/tokenId/pkgName），接收方通过 `CheckAclData` 校验。本地调用方（`callingDeviceID` 为空或等于本地 `networkId`）走权限校验，远端调用方走 ACL 校验。
- **分布式数据同步**：`DistributedDataStorage` 使用 KV Store 进行跨设备数据同步，`SyncAndCompleted` 是同步关键路径，通过 `DistributedDataStorageCallback` 的 `promise` 等待同步结果（超时 3 分钟）。数据 Key 格式为 `udid_bundleName`，Value 为 `DistributedBundleInfo` 的 JSON 字符串。修改 `DistributedBundleInfo` 字段或 JSON 序列化格式须考虑跨版本数据兼容性：旧版本设备可能存储或读取旧格式数据，新增字段须可选、删除字段须有降级处理，避免反序列化失败导致数据丢失。
- **IPC 消息码注册**：新增 IPC 消息码必须同时在 `DistributedInterfaceCode` 枚举和 `DistributedBmsHost::OnRemoteRequest` 的 switch 分发中同步完成，缺一则 IPC 不完整。
- **错误码属于公共兼容性边界**：新增错误码必须归入 `appexecfwk_errors.h` 或 `bundle_manager` 错误码的已有模块偏移。
- **接口描述符校验**：`OnRemoteRequest` 必须校验 `InterfaceToken`，不匹配返回 `ERR_INVALID_STATE`。
- **批量查询上限**：`GetParcelableInfos` 限制单次最多 10 个元素，超限返回失败。

### Do not

- 不要在日志中明文打印 UDID、UUID 或 networkId；必须使用 `AnonymizeUdid` 脱敏。
- 不要跳过 `VerifySystemApp` 或 `VerifyCallingPermission` 校验来使测试通过。
- 不要绕过 `VerifyCallingPermissionOrAclCheck` 的 ACL 校验链。
- 不要修改或删除已有错误码；新增错误码必须归入已有模块偏移。
- 不要修改 Public API 签名、权限行为或生命周期语义，除非任务明确要求。
- 不要在 `SyncAndCompleted` 中跳过本地 UDID 与远端 UDID 的对比检查。
- 不要在 KV Store 操作中跳过 `CheckKvStore` 空指针检查。
- 不要修改 `dbms.gni` 中的功能开关默认值，除非任务明确要求。
- 不要做与任务无关的重构、格式化或命名调整。
- 不要回滚用户已有改动；遇到工作区脏文件时，只处理与当前任务直接相关的文件。

### Ask before

以下变更必须先向用户确认，不得自行决定：

- 修改 `interfaces/` 下任何对外 API 的签名、语义或错误码。
- 修改权限校验逻辑、ACL 校验链或 IPC 接口协议。
- 修改 `DistributedDataStorage` 的 KV Store 配置（AppId、StoreId、SecurityLevel、数据格式）。
- 新增或修改 IPC 消息码（`DistributedInterfaceCode`）。
- 新增第三方依赖或修改已有依赖版本。
- 修改 `sa_profile/402.json` 的 SA 配置（启动策略、卸载策略）。
- 修改 `dbms.gni` 中的功能开关默认值。
- 修改 `DistributedBmsAclInfo` 的字段或序列化格式。

### 已知易错点

- 新增 IPC 消息码只加枚举未加 Host switch 分发，导致 IPC 不完整。
- 跨设备调用未构建 `DistributedBmsAclInfo` 或未传递给远端，导致 ACL 校验失败。
- `SyncAndCompleted` 中未区分本地 UDID 和远端 UDID，导致本地查询触发不必要的同步。
- 日志中明文打印 UDID/networkId 导致隐私泄露。
- 修改功能开关后未验证构建范围变化（条件编译分支未覆盖）。
- `GetParcelableInfos` 未校验批量上限（10），导致远端 DoS 风险。
- `CheckKvStore` 重试逻辑（600 次 × 100ms）被绕过，导致空指针访问。

## 验证闭环

### 最小验证命令

构建命令从 OpenHarmony 源码根目录执行，不在本子目录执行。

```bash
# 编译验证
./build.sh --product-name rk3568 --build-target distributed_bundle_framework

# 编译服务模块
./build.sh --product-name rk3568 --build-target services/dbms:dbms_target

# 服务单元测试
./build.sh --product-name rk3568 --build-target services/dbms/test:unittest

# Fuzz 测试
./build.sh --product-name rk3568 --build-target test/fuzztest:fuzztest

# 全量测试
./build.sh --product-name rk3568 --build-target test_target
```

如果当前环境缺少 OpenHarmony 构建链、产品配置或依赖仓库，请不要伪造构建结果；在最终说明中明确写出未能运行的命令和原因。

### 静态分析与 Sanitize 检查

项目在 `services/dbms/BUILD.gn` 中已启用以下 sanitize 选项，编译时自动生效：

- `boundary_sanitize`（边界检查）
- `cfi` + `cfi_cross_dso`（控制流完整性）
- `integer_overflow`（整数溢出检查）
- `ubsan`（未定义行为检查）
- `fstack-protector-strong`（栈溢出保护）

如环境支持，可在编译后运行 `cppcheck` 进行补充静态分析：

```bash
cppcheck --enable=warning,performance,portability --std=c++17 services/dbms/src/ interfaces/inner_api/src/
```

关注 sanitize 编译报错和 `cppcheck` 警告，特别是空指针解引用、整数溢出和越界访问。

### 按变更类型验证

| 变更类型 | 最小验证 |
|----------|---------|
| 修改内部实现（services/dbms/src/） | 编译通过 + 相关模块单测 |
| 新增/修改 IPC 方法 | 编译通过 + 搜索 `DistributedInterfaceCode` 确认枚举已注册 + 搜索 Host `OnRemoteRequest` 确认 switch 分发已处理 |
| 新增/修改 Public API（interfaces/kits/） | 编译通过 + 全量单测 + 各语言绑定编译 + 兼容性评估 |
| 修改权限或 ACL 校验 | 编译通过 + 权限相关单测 + 搜索所有调用方确认影响 |
| 修改 `DistributedDataStorage` | 编译通过 + 数据同步相关单测 + 确认 KV Store Key/Value 格式兼容 |
| 修改设备管理或 UDID/UUID 解析 | 编译通过 + 搜索所有 `GetUdidByNetworkId`/`GetUuidByNetworkId` 调用方确认影响 |
| 日志新增/修改 | 确认 UDID/networkId 已脱敏 + 搜索相邻模块日志风格保持一致 |
| 测试变更 | 运行变更的测试 + 至少一个相邻相关测试 |

### Done 定义

一个任务只有同时满足以下条件才算完成：

1. 请求的行为已实现。
2. 相关编译、测试、兼容性验证已运行，或已说明无法运行的原因。
3. `git diff` 仅包含预期改动，无无关重构或格式化。
4. 新增或修改的错误路径有清晰返回值和日志。
5. 修改接口、配置或构建文件时，已检查相关依赖和目标。
6. 日志中涉及的设备标识（UDID/UUID/networkId）已脱敏。

### 最终回复格式

向用户汇报时请包含：

- 改了哪些文件。
- 行为上解决了什么问题。
- 运行了哪些验证及结果。
- 哪些验证因环境限制未运行，以及残留风险。
