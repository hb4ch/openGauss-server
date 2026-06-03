# LibreGauss

> *"把高斯数据库从政企黑盒里解放出来"*

## 这项目是干啥的？

华为 openGauss 是个好数据库。好在哪里呢？好在它依赖：

- 一个神秘的 `binarylibs` 文件夹，里面装着不知道哪个版本的 GCC 7.3 编译的 .so 文件
- openEuler 操作系统（没听说过？正常）
- 五个华为自己魔改的 Kerberos 库（`libkrb5_gauss`、`libgssapi_krb5_gauss`……你数数有几个 `gauss`）
- 一个自产的 `libsecurec`，实现了 "安全" 的 C 标准库函数 —— 只不过你从 apt 装不上
- DCF —— 分布式一致性框架，源码不公开，但你要编译就必须有它的 .h 文件和 .so

翻译成人话：**这玩意儿在 Ubuntu 上默认编译不过。**

LibreGauss 就是来解决这个问题的。

## 我们干了什么

把 openGauss 从华为的"编译黑箱"里拽了出来，让它能在 **Ubuntu 22.04** 上用 **apt install** + **自己写的存根代码** 完成编译。

主要改动：

| 改动 | 说明 |
|------|------|
| `.dockerignore` | 把 793MB 的构建上下文砍到了 ~200MB（你知道 `src/test/` 有 562MB 吗？我也才知道） |
| 华为 Secure C | 我们写了个 `docker/securec_stub.c`，用标准 C 函数实现了那几百个 `_s` 函数 |
| DCF | 写了个 `docker/dcf_interface.h` 和 `docker/libdcf_stub.c`，40 多个函数全部返回 0 —— 分布式一致性？不存在的，但能编译 |
| ONNX Runtime | `docker/onnx_stubs.c` —— 11 行代码，实现了全部 ONNX 接口（开玩笑的，全返回 NULL） |
| XGBoost | `binarylibs/kernel/dependency/xgboost/comm/include/xgboost/c_api.h` —— 三个 typedef，搞定 |
| GCC 兼容性 | 修了 `gettimeofday(NULL)` 歧义、`-fexcess-precision` 不支持、`-fpermissive`、AVX512 内联失败……GCC 15 的锅，我们背了 |
| 编译脚本 | `docker_build.sh` —— 持久化容器编译，第一次 30 分钟，第二次 30 秒 |

## 快速开始

```bash
# 第一次构建（去泡杯咖啡，或者泡个面）
./docker_build.sh

# 只重新编译改过的文件
./docker_build.sh make

# 如果真想跑起来……
# 抱歉，initdb 会在创建 template1 时 segfault
# 因为我们那些存根函数返回 NULL 的地方，华为代码期望的是一个有效指针
# 这叫"能编译但跑不起来"，简称"毕业设计"
```

## 技术细节（不想看的可以跳过）

### 构建系统架构

```
                  ┌─────────────────────┐
                  │   Dockerfile.dev     │
                  │  (中国镜像 tuna.tsinghua) │
                  └────────┬────────────┘
                           │ docker build (一次)
                           ▼
                  ┌─────────────────────┐
                  │   opengauss-dev 镜像  │
                  │  (apt 依赖全装好)     │
                  └────────┬────────────┘
                           │ docker run -v 源码目录
                           ▼
                  ┌─────────────────────┐
                  │   og-builder 容器     │
                  │  (持久化, 源码卷挂载)   │
                  │                      │
                  │  1. 编译 securec 存根  │
                  │  2. 编译 DCF 存根      │
                  │  3. 编译 ONNX 存根     │
                  │  4. ln -sf 各种库      │
                  │  5. ./configure       │
                  │  6. make -j6          │ ← 30 分钟
                  │  7. cp gaussdb        │
                  └─────────────────────┘
```

### 那些年我们追过的 .so

华为的 `binarylibs` 包含了一堆编译自 CentOS 7 + GCC 7.3 的 `.so` 文件。
在 Ubuntu 22.04 + glibc 2.35 上，它们**能加载，但会段错误**。

这是 glibc 的 ABI 兼容性问题。解决方案？重新编译一切。

### 存根哲学

不能从 apt 安装的，我们就写存根。
存根就是：**函数签名一模一样，函数体全是 `return 0;`**。

这违反直觉，但在"让它编译过"这个目标面前，非常有效。

```
真正的 DCF：     分布式共识，Paxos/Raft，日志复制，领导者选举
我们的 DCF 存根： int dcf_start() { return 0; }  // 好的，已经开始了
                  int dcf_write() { return 0; }   // 好的，已经写完了
```

## 已知问题

- **initdb segfault**：存根函数返回 NULL 的地方，华为代码期望一个正经指针。修起来不难，但我们还没修
- **`gs_ctl` 编译错误**：`pg_basebackup` 等几个工具在 lite 模式下缺少链接库。修了一半
- **`gs_initdb` 找不到文件**：`make install` 因为上面的问题失败了，导致 `share/` 目录不完整。手动拷贝可以绕过
- **没法跑 SQL**：因为步骤 1 就失败了。先有 initdb，后有 CRUD

## 食用建议

如果你真的想跑起来 openGauss：
1. 用 openEuler
2. 用他们的二进制包
3. 或者交钱给华为买支持

如果你就想看看这玩意儿能不能在 Ubuntu 上编译：
1. 用这个 Repo
2. `./docker_build.sh`
3. 欣赏 356MB 的 gaussdb 二进制文件
4. 满意地点点头，然后去干点别的

## 许可证

跟 openGauss 一样，Mulan PSL v2。

不过说真的，这项目能跑起来就是奇迹，别太当真。
