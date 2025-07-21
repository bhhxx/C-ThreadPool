#include <queue>
#include <mutex>
#include <stdexcept>

// 线程安全队列
// 主要实现的功能：进队列，出队列，并保证线程安全
template <typename value_type>
class ThreadSafeQueue {
private:
    std::queue<value_type> queue_; // 线程安全队列
    std::mutex q_mutex_; // 互斥体
public:
    // 右值
    void push(value_type && elem) {
        std::lock_guard<std::mutex> lock(q_mutex_); // RAII 风格加锁
        queue_.push(std::move(elem));
    }
    // 左值
    void push(const value_type & elem) {
        std::lock_guard<std::mutex> lock(q_mutex_);
        queue_.push(elem);
    }
    // pop 需要先判断 queue 是不是空
    value_type pop() {
        std::lock_guard<std::mutex> lock(q_mutex_);
        if (queue_.empty()) {
            throw std::runtime_error("Queue is empty");
        } else {
            value_type val = std::move(queue_.front());
            queue_.pop();
            return std::move(val); 
        }
    }
};


class ThreadPool {
private:

public:
};