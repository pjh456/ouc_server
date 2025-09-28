# 贡献指南

感谢你对 OUC Server 的关注！

为了让团队协作更高效，请在贡献代码前阅读并遵循以下规范。

---

## 🌱 开发流程

1. **Fork 仓库** 到自己的 GitHub 账号下。
2. **同步主仓库** 保持代码最新：
    ```bash
    git fetch upstream
    git checkout dev
    git pull upstream dev
    ```
3. **新建分支** 开发功能
    ```bash
    git checkout -b feature/your-feature-name
    ```
4. 在本地完成开发，并确保编译和测试通过。
5. **提交代码** 到自己 fork 的仓库。
6. 发起 **Pull Request (PR)**，目标分支为 `dev`。
7. 等待 **Code Review**，修改问题后再合并。

---

## 🌳 分支模型

- `main`：始终保持稳定可运行，仅在发布版本时合并。
- `dev`：日常开发分支，所有功能和修复分支都基于此。
- `feature/*`：新功能开发分支，例如 `feature/http-server`。
- `bugfix/*`：修复分支，例如 `bugfix/memory-leak`。

---

## 📝 提交规范（Commit Message）

请遵循以下格式：

```<type>: <description>```

常见 type：
- `feat`：新增功能
- `fix`：修复 bug
- `docs`：文档更新
- `test`：增加/修改测试
- `refactor`：代码重构（无功能新增）
- `perf`：性能优化
- `style`：代码格式调整（空格、缩进等）
- `chore`：构建/配置修改

示例：

```feat: 添加基础 TCP socket 封装```

```fix: 修复 epoll 边缘触发模式下的阻塞问题```

---

## ✅ 测试与检查

- 每个功能/修复都必须附带相应的 **单元测试**。
- 所有 PR 必须通过 **CI 构建**（CMake 编译 + 测试）。
- 推荐在提交前运行：
    ```bash
    mkdir build && cd build
    cmake ..
    make -j4
    ctest
    ```

---

## 🎨 代码风格

- 使用 **C++17** 标准。
- 缩进统一为 **4 空格**。
- 文件名统一小写，单词用下划线分隔（如 `event_loop.h`）。
- 类名采用 **大驼峰命名**（如 `EventLoop`）。
- 函数与变量采用 **小写加下划线**（如 `start_server()`，`buffer_size`）。
- 公共接口需要写清楚 **Doxygen 风格注释**。

---

## 🔍 Code Review 原则

详见 [代码规范](code_style.md)

---

## 📌 贡献者守则

请保持尊重、开放和协作的态度。

任何形式的歧视或攻击性言行都不被允许。

---

感谢你为项目贡献力量！ 🚀

