#ifndef INCLUDE_OUC_SERVER_THREAD_POOL
#define INCLUDE_OUC_SERVER_THREAD_POOL

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <future>
#include <functional>
#include <type_traits>
#include <memory>
#include <utility>

namespace ouc_server
{
    namespace utils
    {
        class ThreadPool
        {
        private:
            std::mutex mtx;
            bool is_stop;
            std::condition_variable cv;

            std::vector<std::thread> workers;
            std::queue<std::function<void()>> tasks;

        public:
            ThreadPool(size_t);
            ~ThreadPool();

        public:
            template <typename Func, typename... Args>
            auto sumbit(Func &&func, Args &&...args)
                -> std::future<typename std::invoke_result_t<Func, Args...>>
            {
                using Ret = typename std::invoke_result_t<Func, Args...>;

                auto task_ptr = std::make_shared<
                    std::packaged_task<Ret()>>(
                    std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

                std::future<Ret> res = task_ptr->get_future();
                {
                    std::unique_lock<std::mutex> lk(mtx);

                    if (is_stop)
                        throw std::runtime_error("ThreadPool has been stopped");

                    tasks.emplace(
                        [task_ptr]()
                        { (*task_ptr)(); });
                }
                cv.notify_one();

                return res;
            }
        };
    }
}

#endif // INCLUDE_OUC_SERVER_THREAD_POOL