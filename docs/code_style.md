# OUC Server 代码规范

本规范适用于 OUC Server 项目，确保团队成员代码风格一致、可读性高、可维护性强。

## 1. C++ 版本与标准

- 使用 **C++17** 或更高版本。
- 优先使用标准库功能，避免非必要的第三方依赖。

## 2. 文件与目录命名

- 文件名统一小写，单词间用下划线分隔：`event_loop.h`、`tcp_server.cpp`
- 目录名小写：`include/`、`src/`、`tests/`、`examples/`
- 头文件包含保护宏：`INCLUDE_OUCSERVER_` + 无后缀文件名，全部字母大写，单词间用下划线分隔
    ```cpp
    #ifndef INCLUDE_OUCSERVER_EVENT_LOOP
    #define INCLUDE_OUCSERVER_EVENT_LOOP
    // ...
    #endif // INCLUDE_OUCSERVER_EVENT_LOOP
    ```

## 3. 类与命名

- 类名采用 **驼峰命名**：
    ```cpp
    class EventLoop {};
    class TcpServer {};
    ```
- 枚举类型使用 **首字母大写的驼峰命名**：
    ```cpp
    enum class SocketState { Closed, Listening, Connected };
    ```

## 4. 命名空间

**不允许在全局环境下使用 `using namespace`!**

- 所有模块都需要包含于 `ouc_server` 命名空间
- 对于一个子模块，应该再包含于以模块的文件夹名为名的命名空间
    ```cpp
    namespace ouc_server
    {
        namespace event
        {
            // ...
        }
    }
    ```

## 5. 函数与变量

- 函数名使用 **小写加下划线**：
    ```cpp
    void start_server();
    int get_socket_fd() const;
    ```
- 成员变量使用 `m_` 前缀：
    ```cpp
    int m_socket_fd;
    std::string m_host;
    ```
- 局部变量尽量短小有意义，避免滥用缩写。
- 常量使用 **全大写加下划线**：
    ```cpp
    constexpr int MAX_CONNECTIONS = 1024;
    ```

## 6. 缩进与空格

- 缩进统一为 **Tab**，本地开发推荐显示为 4 空格。
- 操作符两边保留空格：
    ```cpp
    int sum = a + b;
    ```
- 控制语句中除单语句外均需要添加大括号作用域：
    - 条件分支
        ```cpp
        if (condition)
            do_something();                     // Right
        if(condition)
            do_something(), do_another_thing(); // Wrong!
        if (condition)
        {
            do_something();
            do_another_thing();
        }
        else
        {
            do_another_thing();
            do_something();
        }
        ```
    - 循环结构
        ```cpp
        for (int i = 0; i < n; ++i)
        {
            do_something();
        }
        while(condition)
        {
            do_something();
        }
        do
        {
            do_something();
        }
        while(condition);
        ```
    - 选择结构
        ```cpp
        switch(condition)
        {
            case 1:
                do_something();
                break;
            //case ...:
            default:
                do_something();
                break;
        }
        ```

## 7. 注释与文档

- 所有 **公共接口** 必须有 Doxygen 风格注释：
    ```cpp
    /**
     * @brief 启动 TCP 服务器
     * @param port 监听端口
     */
    void start_server(int port);
    ```
- 复杂逻辑可以加内联注释，但避免冗余：
    ```cpp
    // 使用非阻塞模式读取数据
    socket.read_nonblocking();
    ```

## 8. 异常与错误处理
- 尽量使用 **RAII** 管理资源，避免手动 delete。
- 遇到严重错误，可使用 `throw` 抛异常；非严重错误返回错误码或状态。

## 9.头文件包含顺序

头文件按照以下顺序引入，不同类型头文件之间用空行隔开：

1. 对应模块的头文件
2. 标准库头文件
3. 第三方库头文件
4. 其他模块头文件

示例：
```cpp
#include "tcp_server.hpp"

#include <vector>
#include <string>
#include <iostream>

#include "third_party/some_c_lib.h"
#include "third_party/some_cpp_lib.hpp"

#include "event_loop.hpp"
```

## 10. 测试与调试

- 每个模块必须有对应单元测试。
- 调试代码在提交前必须移除或用宏控制：
    ```cpp
    #ifdef DEBUG
    std::cout << "Debug info" << std::endl;
    #endif
    ```

## 11. 格式化工具（推荐）

- 可使用 `clang-format` 自动格式化代码，推荐团队统一配置 `.clang-format` 文件。
- 所有 PR 提交前请运行格式化，保证一致性。