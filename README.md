# OUC Server

本项目由中国海洋大学爱特工作室组织开发，目标是基于 Linux 构建一个 **高性能、可扩展的现代 C++ 网络编程库**，为学习和实践网络编程提供平台，同时探索事件驱动架构、异步 I/O、协议封装等关键技术。

## ✨ 项目目标

- 提供简洁易用的 **Socket 封装** 接口
- 支持 **多路复用**（epoll/kqueue等）
- 内置 **事件循环** 与 **任务调度**
- 扩展常用协议（HTTP、WebSocket等）
- 作为教学和研究平台，便于社团成员学习与贡献

## 🚀 快速开始

### 环境要求

- C++ 17 或更高版本
- CMake $\geq$ 3.15
- Linux 环境（推荐 Debian 12）
- （可选）Docker 用于统一开发环境

### ⚙️ 构建步骤

```bash
git clone https://github.com/ITStudioOUC/ouc_server
cd ouc_server
mkdir build && cd build
cmake ..
make -j4
```

### 📖 运行示例

```bash
./examples
```

### 📂 仓库结构

```bash
ouc_server
├── include/ # 公共头文件
├── src/ # 源代码
├── examples/ # 示例程序
├── tests/ # 单元测试 
├── docs/ # 文档 
├── CMakeLists.txt 
└── README.md
```

### 🤝 参与贡献

我们欢迎任何形式的贡献！

1. Fork 本仓库
2. 从 dev 分支新建功能分支 feature/...
3. 提交 Pull Request，并等待 Review
4. 通过 CI 检查和 Review 后合并到主仓库

具体规范请参考 [CONTRIBUTING.md](/docs/CONTRIBUTING.md)。

## 📌 开发路线图

- [ ] 完成基本 TCP/UDP 封装
- [ ] 引入 epoll/kqueue 事件循环
- [ ] 提供线程池/协程调度器
- [ ] 增加 HTTP 协议支持
- [ ] 编写性能测试与文档

## 📚 学习资料

哈哈，没写

## 📜 许可证
本项目采用 [MIT License](/LICENSE) 开源。