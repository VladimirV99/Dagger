#pragma once

#include "common.h"
#include "message.h"

#include <mutex>
#include <queue>

using namespace dagger;

template <typename T>
class ConcurrentQueue
{
public:
    ConcurrentQueue() = default;
    ConcurrentQueue(const ConcurrentQueue<T>&) = delete;

    virtual ~ConcurrentQueue()
    {
        Clear();
    }

    const T& Front()
    {
        std::scoped_lock lock(m_queueMutex);
        return m_queue.front();
    }

    const T& Back()
    {
        std::scoped_lock lock(m_queueMutex);
        return m_queue.back();
    }

    void Push(const T& element_)
    {
        std::scoped_lock lock(m_queueMutex);
        m_queue.emplace(std::move(element_));

        std::unique_lock<std::mutex> ul(m_blockingMutex);
        m_blocking.notify_one();
    }

    T Pop()
    {
        std::scoped_lock lock(m_queueMutex);
        auto e = std::move(m_queue.front());
        m_queue.pop();
        return e;
    }

    bool Empty()
    {
        std::scoped_lock lock(m_queueMutex);
        return m_queue.empty();
    }

    size_t Size()
    {
        std::scoped_lock lock(m_queueMutex);
        return m_queue.size();
    }

    void Clear()
    {
        std::scoped_lock lock(m_queueMutex);
        while(!m_queue.empty())
            m_queue.pop();
    }

    void wait()
    {
        while(Empty())
        {
            std::unique_lock<std::mutex> ul(m_blockingMutex);
            m_blocking.wait(ul);
        }
    }

private:
    std::mutex m_queueMutex;
    std::queue<T> m_queue;

    std::condition_variable m_blocking;
    std::mutex m_blockingMutex;
};
