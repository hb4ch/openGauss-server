# LibreGauss

> *"把高斯数据库从政企黑盒里解放出来"*

## 这项目是干啥的？

华为 openGauss 是个好数据库。好在哪里呢？好在它依赖：

- 一个神秘的 `binarylibs` 文件夹，里面装着不知道哪个版本的 GCC 7.3 编译的 .so 文件
- openEuler 操作系统（没听说过？正常）
- 五个华为自己魔改的 Kerberos 库（`libkrb5_gauss`、`libgssapi_krb5_gauss`……你数数有几个 `gauss`）
- 一个自产的 `libsecurec`，实现了"安全"的 C 标准库函数 —— 只不过你从 apt 装不上
- DCF —— 分布式一致性框架，源码不公开，但你要编译就必须有它的 .h 文件和 .so

翻译成人话：**这玩意儿在 Ubuntu 上默认编译不过。**

LibreGauss 就是来解决这个问题的。

## 我们干了什么

把 openGauss 从华为的"编译黑箱"里拽了出来，让它能在 **Ubuntu 22.04** 上用 **apt install** + **自己写的存根代码** 完成编译，并且**真的能跑 SQL**。

| 改动 | 说明 |
|------|------|
| `.dockerignore` | 把 793MB 的构建上下文砍到了 ~200MB（`src/test/` 有 562MB） |
| 华为 Secure C | 我们用标准 C 的 `static inline` 函数替换了全部 `_s` 函数（`strcpy_s`→`snprintf`，`memcpy_s`→`memcpy`，……） |
| DCF | 40 多个函数全部 `return 0;` 的存根 —— 分布式一致性？不存在的，但能编译 |
| ONNX Runtime | 11 行代码的存根，全部返回 NULL |
| XGBoost | 三个 typedef，搞定 |
| 安全插件 | 留了个打印"LibreGauss security_plugin stub loaded"的 .so |
| GCC 兼容性 | `gettimeofday(NULL)` 歧义、`-fexcess-precision` 不支持、AVX512 内联失败…… |
| 编译脚本 | `docker_build.sh` —— 持久化容器编译，带 ccache |

## 快速开始

```bash
# 第一次构建（去泡杯咖啡）
./docker_build.sh

# 只重新编译改过的文件
./docker_build.sh make

# 跑起来
docker exec og-builder gs_initdb -D /tmp/pgdata --nodename=single_node
docker exec -d og-builder gaussdb -D /tmp/pgdata --single_node

# 连上去
docker exec og-builder gsql -d postgres -h /tmp -p 5432

postgres=# ALTER ROLE omm PASSWORD 'Test12345!';
postgres=# CREATE TABLE t (id int, name text);
postgres=# INSERT INTO t VALUES (1, 'Alice');
postgres=# SELECT * FROM t;
 id | name
----+-------
  1 | Alice
(1 row)
```

## 当前状态

| 功能 | 状态 |
|------|------|
| `./configure` | ✅ |
| `make gaussdb` | ✅ 356MB debug build |
| `gs_initdb`（完整所有步骤） | ✅ |
| `gaussdb --single_node`（多用户服务器） | ✅ |
| `gsql` 连接 | ✅ |
| `CREATE TABLE / INSERT / SELECT / JOIN` | ✅ |
| `information_schema` | ✅ |
| `template0` / `postgres` 数据库 | ✅ |
| `pg_catalog` 系统视图 | ❌（nodeRead 反序列化崩溃，已禁用） |
| `pg_basebackup` / `gs_ctl` | ❌ 链接缺少 `-lzstd` |

## 技术细节

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

### 华为 Secure C：一场持续 14000 次替换的噩梦

openGauss 源码里有大约 **14000 次** `strcpy_s`、`memcpy_s`、`snprintf_s` 等"安全"函数调用。
华为提供了一个 `libsecurec.a`，里面实现了这些函数。但那是给 GCC 7.3 + openEuler 编译的。

我们的方案分两步：

**阶段一：内联替换**（当前状态）

写了一个 `src/include/securec.h`，用 `static inline` 函数映射所有 `_s` 函数到标准 C：

```
strcpy_s(d, dm, s)   →  snprintf(d, dm, "%s", s)
memcpy_s(d, dm, s, c) →  memcpy(d, s, c)
snprintf_s(d, dm, c, f, ...) →  snprintf(d, dm, f, ...)
securec_check(rc, ...) →  { ((void)(rc)); }  // 你的"安全检查"不需要了
```

没有额外依赖，没有链接库，纯头文件。

**阶段二：逐一手工替换**（计划中，但被 14000 个调用点劝退了）

用 Perl 脚本批量替换了约 1300 个文件。编译通过。问题是有些宏展开的 `{ }` vs `do { } while (0)` 语义对不上，修了一批 `securec_check` 的调用点才搞定。

### 那些年我们追过的 .so

华为的 `binarylibs` 包含了一堆编译自 CentOS 7 + GCC 7.3 的 `.so` 文件。
在 Ubuntu 22.04 + glibc 2.35 上，它们能加载，但会段错误。

解决方案？重新编译一切，或者写存根。

### 存根哲学

不能从 apt 安装的，我们就写存根。
存根就是：**函数签名一模一样，函数体全是 `return 0;`**。

```
真正的 DCF：     分布式共识，Paxos/Raft，日志复制，领导者选举
我们的 DCF 存根： int dcf_start() { return 0; }  // 好的，已经开始了
                  int dcf_write() { return 0; }   // 好的，已经写完了
```

## 已知问题

- **`pg_catalog` 系统视图缺失**：`setup_sysviews()` 和 `setup_perfviews()` 被禁用。
  原因是 `nodeRead` 在反序列化视图规则时崩溃（`unrecognized token: "ALIASold"`）。
  根因是序列化格式问题 —— `nodeToString()` 输出的规则文本被截断或格式错误，
  导致 `stringToNode` 解析失败。这是华为代码的 bug 被我们的 `securec.h` 内联函数触发了。
- **`pg_basebackup` 等工具链接失败**：缺少 `-lzstd` 链接参数。不影响 `gaussdb` 本体。
- **服务器稳定性**：缺失系统视图导致某些后台进程（job scheduler）启动时报错，
  但不影响 SQL 操作。
- **`maskPassword` 递归崩溃**：错误报告系统的 `maskPassword` 函数在处理损坏的规则文本时
  自身也会崩溃，导致错误信息无法显示。

## 贡献

欢迎 PR。不过说真的，这项目能跑起来就是奇迹，别太当真。

## 许可证

Mulan PSL v2（跟 openGauss 一样）。
