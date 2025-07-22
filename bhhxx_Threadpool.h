#include <queue>
#include <mutex>
#include <functional>
#include <vector>
#include <thread>
#include <condition_variable>
// 线程安全队列
// 主要实现的功能：进队列，出队列，并保证线程安全，提供生产者消费者模型，条件变量阻塞
template <typename value_type>
class ThreadSafeQueue {
private:
    std::queue<value_type> queue_; // 线程安全队列
    std::mutex q_mutex_; // 互斥体
    std::condition_variable cv_; // 用于生产者、消费者的阻塞 pop
    bool stop_ = false; // 记录是否停止
public:
    // 右值
    void push(value_type && elem) {
        std::lock_guard<std::mutex> lock(q_mutex_); // RAII 风格加锁
        queue_.push(std::move(elem));
        cv_.notify_one();
    }
    // 左值：此处可以用 const 是因为 queue_ 的值不需要修改
    void push(const value_type & elem) {
        std::lock_guard<std::mutex> lock(q_mutex_);
        queue_.push(elem);
        cv_.notify_one();
    }
    // pop 需要先判断 queue 是不是空，空的时候阻塞
    bool pop(value_type& value) {
        std::unique_lock<std::mutex> lock(q_mutex_);
        cv_.wait(lock, [this] { return stop_ || !queue_.empty(); });
        if (stop_ && queue_.empty()) {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }
    bool empty() {
        std::lock_guard<std::mutex> lock(q_mutex_);
        return queue_.empty();
    }
    void stop() {
        std::lock_guard<std::mutex> lock(q_mutex_);
        stop_ = true;
        cv_.notify_all();
    }
};

// 线程池，负责提交任务、执行任务
class ThreadPool {
private:
    size_t m_workers_; // 最大线程数量
    ThreadSafeQueue<std::function<void()>> tasks_; // 任务队列
    std::vector<std::thread> workers_; // 工作线程
private:
    // 消费者
    void worker_loop() {
        while (true) {
            // 取出任务
            std::function<void()> task;
            if (!tasks_.pop(task)) {
                return;
            }
            // 执行任务
            task();
        }
    }
public:
    // 构造函数
    ThreadPool(size_t m_workers): m_workers_(m_workers) {
        // 初始化线程
        workers_.resize(m_workers_);
        for (int i = 0; i < m_workers_; i++) {
            workers_[i] = std::thread(&ThreadPool::worker_loop, this); // 传入 worker_loop 函数
        }
    }
    // 析构函数 需要等待多个线程执行完
    ~ThreadPool() {
        tasks_.stop();
        for (auto& t : workers_) {
            if (t.joinable()) t.join();
        }
    }
    // 提交任务，生产者
    void submit(std::function<void()>&& func) {
        // 加入任务
        tasks_.push(std::move(func));
    }
};