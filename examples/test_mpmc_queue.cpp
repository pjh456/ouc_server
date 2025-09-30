#include <utils/mpmc_queue.hpp>

#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
#include <cassert>
#include <chrono>

constexpr size_t PRODUCER_COUNT = 4;
constexpr size_t CONSUMER_COUNT = 4;
constexpr size_t ITEMS_PER_PRODUCER = 100;

int main()
{
    pjh_std::MPMCQueue<int> queue(1024);

    // 生产者线程
    std::vector<std::thread> producers;
    for (size_t i = 0; i < PRODUCER_COUNT; ++i)
    {
        producers.emplace_back(
            [i, &queue]()
            {
            for (size_t j = 0; j < ITEMS_PER_PRODUCER; ++j) {
                // 非阻塞 push，失败就重试
                while (!queue.push(static_cast<int>(i * ITEMS_PER_PRODUCER + j))) {
                    std::this_thread::yield();
                }
            } });
    }

    // 消费者线程
    std::vector<std::thread> consumers;
    std::atomic<size_t> consumed{0};
    for (size_t i = 0; i < CONSUMER_COUNT; ++i)
    {
        consumers.emplace_back(
            [&queue, &consumed]()
            {
            while (true) {
                auto val = queue.pop();
                if (val) {
                    consumed.fetch_add(1, std::memory_order_relaxed);
                } else {
                    // 队列空，检查是否已经完成
                    if (consumed.load(std::memory_order_relaxed) >= PRODUCER_COUNT * ITEMS_PER_PRODUCER)
                        break;
                    std::this_thread::yield();
                }
            } });
    }

    for (auto &t : producers)
        if (t.joinable())
            t.join();

    for (auto &t : consumers)
        if (t.joinable())
            t.join();

    std::cout << "Total produced: " << PRODUCER_COUNT * ITEMS_PER_PRODUCER << "\n";
    std::cout << "Total consumed: " << consumed.load() << "\n";

    if (consumed.load() == PRODUCER_COUNT * ITEMS_PER_PRODUCER)
        std::cout << "Test passed.\n";
    else
        std::cout << "Test failed.\n";

    return 0;
}