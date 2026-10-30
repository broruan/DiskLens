# Validation scope / 验证范围

## Version 1.4.1 / 版本 1.4.1

On 2026-10-30, the sibling-visibility regression reproduced the previous defect: a fixture with Documents, Downloads, Desktop and a file displayed only one of four immediate entries when the node budget was exhausted. The corrected traversal retains all four entries, explicitly marks pending/partial measurements, and allows pending folders to be scanned separately. A read-only comparison against the local home directory found 75 immediate entries and all 75 in the scan result, with zero missing; the 100,000-node bound still left 34 directories pending. This verifies visibility, not complete home-folder size accounting.

All 13 local CTest targets passed, including sibling visibility, hidden/link entries, parent-scope filter reset, continued scanning and CSV states. The production executable passed four Space Analysis UI checks (English/Chinese × 13/18 px) plus a read-only real-home smoke check. The fresh Ubuntu 22.04 amd64 DEB passed metadata, dependencies, ELF, desktop assets, checksums and version checks; its extracted executable passed all seven pages in both languages at 18 px (14 checks). All 549 translations and placeholders passed validation. Native release results and public download verification must be checked for the v1.4.1 tag; these local/offscreen results do not establish physical desktop or GPU performance.

2026-10-30，先用 Documents、Downloads、Desktop 和一个文件复现旧问题：达到节点上限后，四个同级项目仅显示一个。修复后四项均保留，明确标记待统计或部分统计，待统计目录可单独继续扫描。对本机实际主目录只读核对：75 个顶层项目全部进入结果，遗漏 0 项；十万节点上限仍留下 34 个待统计目录。这验证了可见性，不代表主目录总大小已完整统计。

本机 13 个 CTest 目标全部通过，覆盖同级项目、隐藏项与链接、上一级清除搜索、继续扫描及 CSV 状态。正式程序通过空间分析页中英文 × 13/18 px 四组界面检查，以及实际主目录只读检查；全新 Ubuntu 22.04 amd64 DEB 通过元数据、依赖、ELF、桌面资源、校验与版本检查，解包程序通过七类页面中英文 18 px 共 14 组检查；549 条翻译与占位符验证通过。原生发布与公开安装包结果需对应 v1.4.1 标签另行核实；本地离屏检查不代表实体桌面或 GPU 性能。

## Version 1.4.0 / 版本 1.4.0

On 2026-10-29, a fresh local Ubuntu 22.04 amd64 / Qt 5.15.3 release build passed all 13 CTest targets. The generated `disklens_1.4.0_ubuntu22.04_amd64.deb` passed metadata, ELF architecture, dependencies, desktop/icon assets, checksums and `--version` checks. Its extracted executable passed 14 read-only smoke checks: all seven pages in English and Chinese at 18 px, using disposable scan/cache/state directories. All 540 translation messages and placeholders were verified. [Release CI #13](https://github.com/broruan/DiskLens/actions/runs/40844679224), tag `v1.4.0`, commit `119b775`, completed successfully in 3m 24s. All six native Ubuntu 22.04 / 24.04 / 26.04 × amd64 / arm64 jobs passed, followed by release publication. All six DEBs were downloaded again from the public [v1.4.0 release](https://github.com/broruan/DiskLens/releases/tag/v1.4.0) and independently verified against `SHA256SUMS`, version 1.4.0 and their architecture metadata. GitHub latest resolves to v1.4.0. These remain automated/offscreen checks; physical desktop/GPU behavior and real uninstall operations require separate validation.

2026-10-29，本机 Ubuntu 22.04 amd64 / Qt 5.15.3 全新发布构建通过 13 个 CTest 目标。生成的 v1.4.0 DEB 通过元数据、ELF 架构、依赖、桌面/图标资源、校验与版本号检查；解包后的程序使用临时目录，通过全部七类页面的中英文 18 px 共 14 组只读检查。540 条翻译及占位符检查通过。[发布 CI #13](https://github.com/broruan/DiskLens/actions/runs/40844679224) 对应标签 `v1.4.0`、提交 `119b775`，用时 3 分 24 秒，Ubuntu 22.04 / 24.04 / 26.04 × amd64 / arm64 六个原生任务及发布任务全部通过。已从公开 Release 重新下载六个 DEB，逐一核对 SHA-256、1.4.0 版本号及架构元数据；最新版本入口已指向 v1.4.0。以上为自动化及离屏验证，不代表实体桌面/GPU 或真实卸载验证。下方保留此前界面检查与历史版本记录。

## Main branch UI refresh — 2026-10-27 / 主分支界面优化

Local validation on Ubuntu 22.04 amd64, GCC 11.3 and Qt 5.15.3:

- Release build and all 13 CTest targets passed. The new behavior checks cover search coalescing/Enter/clear, keyboard navigation and F5 large-file rescan, plus eight English/Chinese × 13/18 px × 1080/1280 px overview layouts.
- 28 read-only smoke runs passed: seven pages (including duplicates) × two languages × two font sizes, using disposable scan/cache/state directories and the production Fusion style. Screenshots were reviewed for the overview, application table, cache table, file table, directory analysis, developer view and appearance dialog.
- Translation coverage remains 540 messages; placeholder checks and `git diff --check` passed. Existing cache checkbox, whole-row click, Space and focused/unfocused contrast checks passed.
- These are offscreen GUI and temporary-fixture checks. No manual desktop/GPU performance benchmark, real-cache cleanup or real package removal is claimed. The updated screenshots describe the main branch; released v1.3.0 binaries are unchanged.

本地环境为 Ubuntu 22.04 amd64、GCC 11.3、Qt 5.15.3。Release 构建与 13 个 CTest 目标全部通过；新增检查覆盖搜索合并/回车/清空、快捷键与大文件 F5 重扫，以及中英文、13/18 px、1080/1280 px 窗口宽度的八种首页组合。七个页面（含重复文件）、两种语言、两档字号共 28 组离屏只读检查通过，使用临时扫描/缓存/状态目录，并核对了正式样式截图。540 条翻译及占位符、缓存勾选/空格/选中对比度回归均通过。上述结果不代表人工桌面/GPU 性能基准或真实卸载/用户缓存清理验证；截图对应 main 分支，v1.3.0 发布包未改动。

[Build instructions](building.md) · [CI runs](https://github.com/broruan/DiskLens/actions/workflows/build.yml)

## Version 1.3.0 / 版本 1.3.0

[Release CI #10](https://github.com/broruan/DiskLens/actions/runs/39109060208), tag `v1.3.0`, commit `e4a7b8f`: all six native combinations passed on 2026-10-18 — Ubuntu 22.04 / 24.04 / 26.04 × amd64 / arm64. Each ran all 13 CTest targets, verified and installed its DEB, checked all seven pages in both languages, and checked overview/cache/explorer/developer at English 18 px. The release completed in 2m 30s and published six DEBs plus SHA256SUMS. All six published downloads were independently verified against SHA-256 and version/architecture metadata. The same commit also passed the [main-branch run](https://github.com/broruan/DiskLens/actions/runs/39109059959).

2026-10-18，[发布 CI #10](https://github.com/broruan/DiskLens/actions/runs/39109060208) 在 `v1.3.0`、`e4a7b8f` 上完成六个原生平台检查：Ubuntu 22.04 / 24.04 / 26.04 × amd64 / arm64 全部通过。每个平台均执行 13 个 CTest 目标、DEB 检查与安装，以及七类页面的双语检查和总览、缓存、目录、开发页面的英文 18 px 检查。发布共用时 2 分 30 秒，生成六个 DEB 与 SHA256SUMS；发布后重新下载，六个包的 SHA-256、版本和架构全部正确。同一提交的 [主分支构建](https://github.com/broruan/DiskLens/actions/runs/39109059959) 也已通过。

Version 1.3.0 adds in-app English / Simplified Chinese switching, persistent font size preferences and session-only command-line overrides. Its targeted checks cover translation completeness and placeholders, preference persistence, production-style font scaling, visible language settings and safe appearance-change boundaries. The full suite now contains 13 CTest targets. Local Ubuntu 22.04 amd64 / Qt 5.15.3 checks passed all 13 targets, compiled translation coverage for 540 messages, and all seven pages in Chinese 13 px, English 13 px and English 18 px with isolated fixtures. The 1.2.0 results below remain historical evidence.

1.3.0 新增应用内中英文切换、字体大小偏好保存与仅本次启动的命令行覆盖。针对性检查覆盖译文完整性与占位符、偏好持久化、正式样式字体缩放、可见语言设置入口，以及外观切换的操作边界；完整测试现为 13 个 CTest 目标。本机 Ubuntu 22.04 amd64 / Qt 5.15.3 已通过全部 13 项、540 条编译译文检查，以及隔离样例下七类页面的中文 13 px、英文 13 px、英文 18 px 检查。下方 1.2.0 记录保留为历史证据。

## Version 1.2.0 / 版本 1.2.0

[Release CI #6](https://github.com/broruan/DiskLens/actions/runs/39090501349) passed on 2026-10-18 for tag `v1.2.0`, commit `ab2ade5`. **All six combinations passed**: Ubuntu 22.04, 24.04 and 26.04, each on amd64 and arm64. Each job ran all 11 CTest targets, verified and installed its DEB, and ran installed-app read-only smoke checks for overview, explorer, duplicates and developer pages. The release job published six DEBs and `SHA256SUMS`; all six published downloads were independently checked against their SHA-256 values and version/architecture metadata.

2026-10-18，[正式发布 CI #6](https://github.com/broruan/DiskLens/actions/runs/39090501349) 在标签 `v1.2.0`、提交 `ab2ade5` 上通过。Ubuntu 22.04、24.04、26.04 的 amd64 与 arm64 **六个组合全部通过**：各自完成 11 个 CTest 目标、DEB 检查、安装及四类新旧页面的安装后只读检查。发布任务生成六个安装包和校验文件；发布后重新下载六个包，SHA-256 与版本、架构元数据均验证正确。

Local Ubuntu 22.04 amd64 / Qt 5.15.3: all **11 CTest targets** passed, and the updated application passed read-only smoke checks for overview, cache, explorer, duplicates and developer pages using disposable fixtures. The previous 1.1.2 evidence below remains historical.

本机 Ubuntu 22.04 amd64 / Qt 5.15.3 的 **11 个 CTest 目标**全部通过，使用临时样例对总览、缓存、目录分析、重复文件和开发环境页面运行只读检查并截图。下方 1.1.2 记录保留为历史证据。

## Release v1.1.2 / 已验证发布

On 2026-10-18, [release CI run #5](https://github.com/broruan/DiskLens/actions/runs/39088717438) completed successfully for tag [`v1.1.2`](https://github.com/broruan/DiskLens/tree/v1.1.2), commit `eb7fa7a`. All six native jobs completed compilation, all five CTest targets, DEB verification, package installation, and a read-only offscreen smoke check. Release assets were downloaded again and all six DEBs matched `SHA256SUMS`.

2026-10-18，[正式发布 CI #5](https://github.com/broruan/DiskLens/actions/runs/39088717438) 在标签 `v1.1.2`、提交 `eb7fa7a` 上全部通过。六个原生任务均完成编译、五个 CTest 目标、DEB 检查、安装包安装与离屏只读界面检查；发布后重新下载六个 DEB，全部通过 `SHA256SUMS` 校验。

| Ubuntu | amd64 | arm64 |
| --- | --- | --- |
| 22.04 | Passed / 通过 | Passed / 通过 |
| 24.04 | Passed / 通过 | Passed / 通过 |
| 26.04 | Passed / 通过 | Passed / 通过 |

These are automated CI results; the desktop and device boundaries below still apply. / 以上为自动化 CI 结果，下述桌面与实机验证边界仍然适用。

## English

The release workflow targets six native builds: Ubuntu 22.04, 24.04, and 26.04, each on `amd64` and `arm64`. The workflow definition is a target matrix; a successful run for the exact release tag is the evidence that its jobs passed. Read individual test logs for conditional skips. Do not infer a green result from the presence of this document or a downloadable package alone.

### Local verification recorded on 2026-10-18

For version 1.1.2 on Ubuntu 22.04 `amd64` with Qt 5, a fresh release build passed all five CTest targets. The generated package was unpacked and checked for metadata, ELF architecture, dependency declarations, desktop / icon assets, checksums, and application version. Read-only offscreen smoke checks passed for the overview and cache pages, producing the README screenshots from disposable demonstration directories. This local record does not claim that the package was installed on every target or that removal / real-cache cleanup was executed.

### What automated checks establish

| Check | Scope |
| --- | --- |
| `scanner` | Thresholds, largest-file limits, hidden files, links, sparse files, mount / permission handling, and cancellation in controlled fixtures. |
| `inventory` | Installed package queries, size and desktop metadata handling, and Flatpak parsing. Optional integrations depend on the host environment. |
| `uninstall` | Identifier validation, protected-component checks, preview lifecycle, and harmless read-only APT simulation where its fixture application exists. It does not perform real uninstall. |
| `cache` | Cache / log classification; selected-file deletion in temporary directories; unchanged-file checks; links, replacement, ownership, mount / root and manifest validation; cancellation. |
| `cachepage` | Production Fusion style and stylesheet; visible checkbox and whole-row clicks; Space selection; focused / unfocused selection contrast; filtering, confirmation, temporary-file deletion, and rescan. |
| `spaceanalyzer` / `explorerpage` | Directory aggregation and navigation, complete duplicate verification beyond matching samples, changing-file rejection, resource limits, cancellation, reports and production-style interaction. |
| `developer` / `developerpage` | Structural classification, raw-data descriptions, nested/hard-link accounting, partial reports and production-style filtering/selection. |
| `appstorage` / `appstoragedialog` | Exact namespaces/catalog, separately measured data, unknown association and styled read-only inspection. |
| `appearance` / `localization` | Language resources and placeholder integrity, saved preferences, font scaling and production-style appearance controls. See the exact release run for completion and skips. |
| Package checks | Debian metadata, declared architecture, bundled executable / desktop entry / icon, generated dependency declarations, and artifact naming. |
| CI package installation and smoke | Installation in the CI environment followed by a read-only offscreen UI check over a temporary directory. |

Cache deletion fixtures use temporary directories. Read-only smoke mode does not trigger cleanup, uninstall, or administrator authorization. The CI inventory includes its runner's software, rather than a representative end-user desktop.

### What needs desktop or device validation

A green CI run does not establish correct behavior of every desktop session, Polkit agent, display server, GPU, filesystem, Snap / Flatpak installation, or physical ARM device. Real authorized DEB / Snap / Flatpak removal, desktop menu and icon rendering, and real cache-cleanup workflows must be checked separately in disposable environments before reporting them as tested.

No claim of real user-cache deletion or authorized package removal is made by the tests above. The latest release notes should distinguish automated results from manual desktop checks.

### Reporting results

Include the release tag or commit, Ubuntu version, architecture, Qt version, exact command, overall result, and skipped cases. A report such as “all five CTest targets passed on Ubuntu 22.04 amd64 with Qt 5” is scoped evidence. It should not be expanded into “all desktops and architectures tested.”

## 简体中文

发布工作流面向六个原生构建组合：Ubuntu 22.04、24.04、26.04，各自包含 `amd64` 与 `arm64`。工作流定义代表目标矩阵，具体发布标签的实际成功记录才是通过证据。应查看单项测试日志里的条件跳过说明；不能仅凭本文存在或有可下载包就推断 CI 已通过。

### 2026-10-18 本机验证记录

1.1.2 在 Ubuntu 22.04 `amd64` / Qt 5 下完成全新 Release 构建，五个 CTest 目标全部通过。生成包已解包检查元数据、ELF 架构、依赖声明、桌面与图标资源、校验和及程序版本。总览和缓存页面的离屏只读检查通过，README 截图来自可丢弃演示目录。此记录不代表已在所有目标系统安装，不包含实际卸载或真实用户缓存删除。

### 自动检查覆盖

| 检查 | 范围 |
| --- | --- |
| `scanner` | 受控样例中的阈值、Top-N、隐藏文件、链接、稀疏文件、挂载与权限处理、取消。 |
| `inventory` | 本机库存、体积与桌面元数据、Flatpak 解析；可选集成依赖测试环境。 |
| `uninstall` | 非法标识、系统组件保护、预览生命周期、存在样例应用时的无害 APT 只读模拟；不实际卸载。 |
| `cache` | 缓存日志分类、临时目录内所选文件删除、文件变化复核、链接与替换、所有权、挂载与根目录、快照校验、取消。 |
| `cachepage` | 正式 Fusion 风格与完整样式；可见复选框、整行和空格勾选；有无焦点时选中数字对比度；筛选、确认、临时文件删除与重扫。 |
| `spaceanalyzer` / `explorerpage` | 目录累加与逐层导航、采样碰撞后的完整哈希、变化文件排除、上限、取消、报告及正式样式交互。 |
| `developer` / `developerpage` | 结构分类、原始数据说明、嵌套与硬链接计量、部分报告及正式样式筛选选择。 |
| `appstorage` / `appstoragedialog` | 精确命名空间和目录规则、独立计量、未知关联说明及只读界面。 |
| `appearance` / `localization` | 语言资源与占位符完整性、偏好保存、字体缩放及正式样式外观控件；完成结果与跳过项以对应发布记录为准。 |
| 安装包检查 | Debian 元数据、架构、可执行文件、桌面入口、图标、依赖声明与产物命名。 |
| CI 安装与只读界面检查 | 在 CI 环境安装生成包，再对临时目录运行离屏只读检查。 |

缓存删除测试仅操作临时目录。只读检查模式不触发清理、卸载或管理员授权。CI 库存来自构建机，不代表用户桌面软件环境。

### 需要独立验证的内容

CI 通过不能证明所有桌面会话、Polkit 代理、显示服务器、GPU、文件系统、Snap / Flatpak 安装或 ARM 实机的行为。真实授权后的 DEB / Snap / Flatpak 卸载、桌面菜单与图标渲染、真实缓存清理流程，应在可丢弃环境分别验证后再报告。

上述测试不构成真实用户缓存清理或授权卸载验证。发布说明应分别列出自动结果与手工桌面验证。

### 如何记录验证

附上标签或提交、Ubuntu 版本、架构、Qt 版本、命令、总体结果和跳过项。例如“Ubuntu 22.04 amd64 / Qt 5 下五个 CTest 目标通过”具有明确范围，不能扩大为“所有桌面与架构已验证”。
