# OUC Server 代码规范

本规范适用于 OUC Server 项目，确保团队成员代码风格一致、可读性高、可维护性强。

## 1. C++ 版本与标准

- 使用 **C++17** 或更高版本。
- 优先使用标准库功能，避免非必要的第三方依赖。
- 引入第三方依赖时，若有 CMake 或 Github 仓库时应避免直接复制文件于本项目。

## 2. 文件与目录命名

- 文件名统一小写，单词间用下划线分隔：`event_loop.h`、`tcp_server.cpp`
- 目录名统一小写，单词间用下划线分隔：`include/`、`src/`、`tests/`、`examples/`
- 头文件包含保护宏：`INCLUDE_OUC_SERVER_` + 无后缀文件名，全部字母大写，单词间用下划线分隔
    ```cpp
    #ifndef INCLUDE_OUC_SERVER_EVENT_LOOP
    #define INCLUDE_OUC_SERVER_EVENT_LOOP
    // ...
    #endif // INCLUDE_OUC_SERVER_EVENT_LOOP
    ```

## 3. 类型、结构体与枚举类型

- 类名采用 **大驼峰命名**：
    ```cpp
    class EventLoop {};
    class TcpServer {};
    ```
- 成员变量使用 `m_` 前缀：
    ```cpp
    int m_socket_fd;
    std::string m_host;
    ```
- 一般情况下 **不允许** 成员变量对外部可见，若有成员变量的读写需求可采取以下方式：
    - 使用 `T get_name() const` 与 `void set_name(const T&)` 获取与设置不需要引用的成员变量。
    > 若有返回错误码的需求，可以改为 `bool set_name(const T&)`。
    - 使用 `T& name()` 与 `const T& name() const` 获取需要引用的成员变量（与前一种方式 **不共存**）。
    - 成员变量的读写应当最大程度地确保 **异常安全**，如果满足条件应添加 `noexcept` 签名。
- 结构体与类型成员变量，若无特殊需求应当放在类型内最前面：
    ```cpp
    class Data
    {
    private:
        int data1;
        char data2;
    public:
        Data() = default;
        // ...
    }
    ```
- 枚举类型使用 **大驼峰命名**：
    ```cpp
    enum class SocketState { Closed, Listening, Connected };
    ```

## 4. 命名空间

**不允许在全局环境下使用 `using namespace`!**

- 所有模块都需要包含于 `ouc_server` 命名空间
- 对于一个子模块，应该再包含于 **以模块的文件夹名为名** 的命名空间
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
- 若与函数名等重名需要区分时，函数形参名统一使用 `p_` 前缀：
    ```cpp
    int data;
    int do_something(int p_data);
    ```
- 局部变量尽量短小有意义，避免滥用缩写。
- 常量使用 **全大写加下划线**：
    ```cpp
    constexpr int MAX_CONNECTIONS = 1024;
    ```
- 函数声明时，对 **拷贝开销大的** 形参类型应尽可能避免非必要类型转换和拷贝，根据以下原则对 **拷贝开销大的类型** 尽量避免使用`T`：
    - 需要拷贝一个值而无需修改参数时，使用 `const T&`
    - 需要传递引用并对参数进行修改时，使用 `T&`
    - 需要移动一个值，原位置不再使用，使用 `T&&`
- 应当避免过多的函数形参个数，若有大量参数配置需要应 **封装对应结构体**。
    ```cpp
    // Wrong!
    int do_with_many_args(int x, int y, int z, bool is_active, bool is_open, char* name, std::string port);
    // Correct
    struct ConfigData
    {
        int x,y,z;
        bool is_active, is_open;
        char* name;
        std::string port;
    };
    int do_with_many_args(const ConfigData& p_config);
    ```
- 函数形参与实参名在有注释或过长时应当 **全部换行处理**，并在第一个参数前换行。
    ```cpp
    int do_something(
        int this_is_a_veeeeeeeeeeeeeeery_long_var,
        int and_this_is_another_veeeeery_long_var,
        int short_but_need_explain_var, /* ... */
        int final_var);
    do_something(
        I_call_thiiiiiiis_function,
        I_call_it_tooooooooooooooo,
        I_need_to_be_explain, /* ... */
        I_am_the_final_one);
    ```

## 6. 缩进与空格

- 缩进统一为 **Tab**，本地开发推荐显示为 4 空格。
- 操作符两边保留空格：
    ```cpp
    int sum = a + b;
    ```
- 控制语句中除单语句及 `do while` 外均需要添加大括号作用域：
    - 条件分支
        ```cpp
        // Wrong!
        if(condition)
            do_something(), do_another_thing();
        // Correct
        if (condition)
            do_something();
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
            do_something();
        for (int i = 0; i < n; ++i)
        {
            do_something();
            do_another_thing();
        }
        while(condition)
            do_something();
        while(condition)
        {
            do_something();
            do_another_thing();
        }
        do
        {
            do_something();
            /* Special: Need {}!*/
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