#include "./Threadpool.h"
#include <iostream>

int test(int a, int b) {
    return a + b;
}

int main() {
    ThreadPool TP(4); // 创建线程池
    std::future<int> future_ = TP.submit(test, 1, 2); // 提交任务
    std::cout << future_.get() << std::endl; // 获取执行结果
    return 0;
}