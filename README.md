# Simple C++17 Thread Pool

A lightweight, header-only, and easy-to-use thread pool implementation in C++17.

## Features ✨

* **Thread-safe** task queue using `std::mutex` and `std::condition_variable`.
* **Modern C++**: Returns a `std::future` from submitted tasks to easily retrieve results.
* **Simple API**: Designed for ease of use and integration.
* **Automatic Shutdown**: Threads are properly joined when the `ThreadPool` object goes out of scope.

## How to Use 🚀

Simply include the header file in your project.

```cpp
#include <iostream>
#include "ThreadPool.h" // Include your thread pool file

// A sample function
int multiply(int a, int b) {
    return a * b;
}

int main() {
    // Create a pool with 4 worker threads
    ThreadPool pool(4);

    // Submit tasks and get futures
    auto future1 = pool.submit([]{ return 10 * 10; });
    auto future2 = pool.submit(multiply, 5, 8);

    // Get the results (this will block until the task is complete)
    int result1 = future1.get();
    int result2 = future2.get();

    std::cout << "Result 1: " << result1 << std::endl; // Output: Result 1: 100
    std::cout << "Result 2: " << result2 << std::endl; // Output: Result 2: 40

    return 0;
}
```
