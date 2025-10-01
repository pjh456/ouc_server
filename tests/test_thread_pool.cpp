#include <utils/thread_pool.hpp>

int main()
{
    using namespace ouc_server::utils;
    ThreadPool pool(4);

    auto f1 = pool.sumbit(
        []()
        { puts("Hello world from thread pool!"); });

    auto f2 = pool.sumbit(
        [](int a, int b)
        {
            return a + b;
        },
        1, 2);

    f1.get();
    printf("f2 result: %d\n", f2.get());
}