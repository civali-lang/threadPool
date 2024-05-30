# 单例模式与线程池应用

## 单例模式介绍

**单例模式**（Singleton Pattern）是一种创建型设计模式，确保一个类只有一个实例，并提供一个全局访问点。这种模式在需要全局状态管理或频繁创建和销毁实例会造成性能问题的场合非常有用。

### 核心特点

- **唯一性**：保证全局只有一个实例对象。
- **全局访问**：提供全局访问点，便于访问实例对象。
- **延迟加载**：通常在第一次被使用时才创建实例。

### 实现要点

- **私有构造函数**：防止外部通过 `new` 创建多个实例。
- **静态私有实例**：类自身负责管理其唯一实例。
- **公有静态方法**：提供全局访问点，返回类的唯一实例。

### 单例模式的实现方式

#### 饿汉式
- 在类加载时就创建实例，实现简单。

#### 懒汉式
- 实例在第一次被使用时才创建，实现延迟加载。

#### 注册式
- 通过一个中央注册表或工厂来管理单例对象的创建和获取。

### 使用单例的优势
- **全局访问点**：整个应用中，单例对象可以通过一个全局访问点进行访问。
- **资源节约**：由于只有一个实例，减少了内存消耗。
- **一致性**：单例对象的状态可以保持一致。

### 适用场景
- 需要全局状态访问点，如**配置管理器**。
- 创建实例需要消耗大量资源，如**线程池**或数据库连接池。

### 常见问题

#### 使用一个全局变量和单例的区别是什么？

- **全局变量**通常只是一个在全局作用域内定义的变量，它没有限制创建实例的数量，也没有提供统一的访问点。
- **单例模式**提供了一个受限的全局访问点，确保了类的实例化只发生一次，并提供了额外的封装和控制。

#### 为什么在多线程环境中实现单例模式更复杂？

- 在**多线程环境**中，需要确保在任何时刻只有一个线程能够创建实例，这通常需要加锁机制，增加了实现的复杂性。

#### 单例模式的线程安全实现有哪些方式？

- **懒汉式**（线程安全）：使用锁（如`mutex`）来同步第一次访问单例实例的代码块。
- **饿汉式**：实例在类加载时就创建，不存在多线程同步问题，自然线程安全。
- **双重检查锁定**（Double-checked Locking）：在懒汉式的基础上，通过双重检查减少锁的开销。

#### 单例模式有什么缺点？

- **耦合性**：单例模式增加了代码之间的耦合性，因为需要通过特定的访问点来获取实例。
- **扩展性**：单例模式的扩展性较差，尤其是当需要支持多种类型的实例时。

## 线程池中的单例模式

线程池管理着一组工作线程，用于执行并发任务。在多线程应用程序中，线程池本身常被设计为**单例**，以确保资源的合理管理和分配。

### 线程池优势

- **资源复用**：减少线程创建和销毁的开销。
- **提高效率**：通过重复利用线程来提高系统效率。
- **控制最大并发数**：避免因大量线程竞争导致的性能下降。

### 单例模式在线程池中的应用

- **全局访问**：确保全局只有一个线程池实例，便于任务调度和管理。
- **线程安全**：单例模式确保线程池在多线程环境下安全地被初始化和访问。

## 示例代码

以下是使用单例模式实现的线程池示例：

```cpp
// ThreadPool.hpp

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

    // 私有构造函数
    ThreadPool(unsigned int threads) : stop(false) {
        for (unsigned int i = 0; i < threads; ++i) {
            workers.emplace_back(&ThreadPool::workerTask, this);
        }
    }

    // 删除拷贝构造函数和赋值操作符
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // 工作线程执行任务的函数
    void workerTask() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                condition.wait(lock, [this] { return stop || !tasks.empty(); });
                if (stop && tasks.empty())
                    return;
                task = std::move(tasks.front());
                tasks.pop();
            }

            task();
        }
    }

public:
    // 获取线程池的单例实例
    static ThreadPool& getInstance(unsigned int threads) {
        static ThreadPool instance(threads);
        return instance;
    }

    // 添加任务到线程池
    void addTask(std::function<void()> task) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) throw std::runtime_error("Adding task to stopped ThreadPool");
        tasks.push(task);
        condition.notify_one();
    }

    // 停止所有线程
    void stopAll() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers) {
            if (worker.joinable()) worker.join();
        }
        workers.clear();
    }

    // 析构函数，停止线程池
    ~ThreadPool() {
        stopAll();
    }
};
#endif // THREAD_POOL_HPP