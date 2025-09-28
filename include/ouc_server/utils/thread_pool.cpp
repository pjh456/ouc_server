#include <utils/thread_pool.hpp>

namespace ouc_server
{
    namespace utils
    {
        ThreadPool::ThreadPool(size_t n)
            : is_stop(false)
        {
            for (size_t idx = 0; idx < n; ++idx)
            {
                workers.reserve(n);
                workers.emplace_back(
                    [this]()
                    {
                        while (true)
                        {
                            std::function<void()> task;

                            {
                                std::unique_lock<std::mutex> lk(this->mtx);

                                this->cv.wait(
                                    lk,
                                    [this]
                                    { return this->is_stop || (!this->tasks.empty()); });

                                if (this->is_stop && this->tasks.empty())
                                    return;

                                task = std::move(this->tasks.front());
                                this->tasks.pop();
                            }

                            task();
                        }
                        return;
                    });
            }
        }

        ThreadPool::~ThreadPool()
        {
            {
                std::lock_guard<std::mutex> lk(mtx);
                is_stop = true;
            }

            cv.notify_all();

            for (auto &worker : workers)
                if (worker.joinable())
                    worker.join();
        }
    }
}