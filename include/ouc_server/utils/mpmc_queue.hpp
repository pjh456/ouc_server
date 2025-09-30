#ifndef INCLUDE_MPMC_QUEUE
#define INCLUDE_MPMC_QUEUE

#include <cstdint>
#include <stdlib.h>
#include <atomic>
#include <utility>
#include <optional>
#include <iostream>

namespace pjh_std
{
    template <typename T>
    class MPMCQueue
    {
    private:
        struct Cell
        {
            std::atomic<size_t> sequence;
            typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
        };
        Cell *m_buffer;

        size_t m_capacity;
        std::atomic<size_t> m_head, m_tail;
        size_t mask;

    public:
        explicit MPMCQueue(size_t p_capacity)
            : m_capacity(2),
              m_buffer(nullptr)
        {
            while (m_capacity < p_capacity)
                m_capacity <<= 1;
            mask = m_capacity - 1;

            m_buffer = new Cell[m_capacity]();
            for (size_t idx = 0; idx < m_capacity; ++idx)
                m_buffer[idx].sequence.store(idx, std::memory_order_relaxed);

            m_head.store(0, std::memory_order_relaxed);
            m_tail.store(0, std::memory_order_relaxed);
        }

        MPMCQueue(const MPMCQueue &) = delete;
        MPMCQueue &operator=(const MPMCQueue &) = delete;

        MPMCQueue(MPMCQueue &&other) noexcept
            : m_capacity(other.m_capacity),
              m_buffer(other.m_buffer),
              m_head(other.m_head),
              m_tail(other.m_tail)
        {
            other.m_capacity = 0;
            other.m_buffer = nullptr;
        }
        MPMCQueue &operator=(MPMCQueue &&other) noexcept
        {
            if (this == &other)
                return *this;

            m_capacity = other.m_capacity, other.m_capacity = 0;
            m_buffer = other.m_buffer, other.m_buffer = nullptr;
            m_head = other.m_head;
            m_tail = other.m_tail;

            return *this;
        }

        ~MPMCQueue()
        {
            for (size_t i = 0; i < m_capacity; ++i)
            {
                size_t seq = m_buffer[i].sequence.load(std::memory_order_relaxed);
                if (seq <= m_tail)
                {
                    T *ptr = reinterpret_cast<T *>(&m_buffer[i].storage);
                    ptr->~T();
                }
            }
            delete[] m_buffer;
        }

    public:
        bool push(const T &val) { return push_impl(val); }
        bool push(T &&val) { return push_impl(std::move(val)); }

        std::optional<T> pop()
        {
            Cell *cell;
            size_t pos = m_head.load(std::memory_order_relaxed);

            while (true)
            {
                cell = &m_buffer[pos & mask];
                size_t seq = cell->sequence.load(std::memory_order_acquire);
                intptr_t diff = (intptr_t)seq - (intptr_t)(pos + m_capacity);

                if (diff == 0)
                {
                    if (m_head.compare_exchange_weak(
                            pos,
                            pos + 1,
                            std::memory_order_acq_rel,
                            std::memory_order_relaxed))
                        break;
                }
                else if (diff < 0)
                    return std::nullopt;
                else
                    pos = m_head.load(std::memory_order_relaxed);
            }

            T *ptr = reinterpret_cast<T *>(&cell->storage);
            T result = std::move(*ptr);
            ptr->~T();

            cell->sequence.store(pos + m_capacity, std::memory_order_release);
            return result;
        }

    public:
        template <typename U>
        bool push_impl(U &&val)
        {
            Cell *cell;
            size_t pos = m_tail.load(std::memory_order_relaxed);

            while (true)
            {
                cell = &m_buffer[pos & mask];
                size_t seq = cell->sequence.load(std::memory_order_acquire);
                intptr_t diff = (intptr_t)seq - (intptr_t)pos;

                if (diff == 0)
                {
                    if (m_tail.compare_exchange_weak(
                            pos,
                            pos + 1,
                            std::memory_order_acq_rel,
                            std::memory_order_relaxed))
                        break;
                }
                else if (diff < 0)
                    return false;
                else
                    pos = m_tail.load(std::memory_order_relaxed);
            }

            try
            {
                new (&cell->storage) U(std::forward<U>(val));
                cell->sequence.store(pos + m_capacity, std::memory_order_release);
            }
            catch (...)
            {
                cell->sequence.store(pos, std::memory_order_release);
                throw;
            }

            return true;
        }
    };
}

#endif