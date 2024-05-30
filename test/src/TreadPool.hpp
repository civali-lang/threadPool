// #include <condition_variable>
// #include <functional>
// #include <future>
// #include <iostream>
// #include <mutex>
// #include <queue>
// #include <stdexcept>
// #include <thread>
// #include <vector>

// // 线程池类定义
// class ThreadPool {
//    private:
//     std::vector<std::thread> workers;
//     std::queue<std::function<void()>> tasks;
//     std::mutex queue_mutex;
//     std::condition_variable condition;
//     bool stop;

//     // 私有构造函数
//     ThreadPool(unsigned int threads) : stop(false) {
//         for (unsigned int i = 0; i < threads; ++i) {
//             workers.emplace_back(&ThreadPool::workerTask, this);
//         }
//     }

//     // 删除拷贝构造函数和赋值操作符
//     ThreadPool(const ThreadPool&) = delete;
//     ThreadPool& operator=(const ThreadPool&) = delete;

//     // 工作线程执行任务的函数
//     void workerTask() {
//         while (true) {
//             std::function<void()> task;
//             {
//                 std::unique_lock<std::mutex> lock(this->queue_mutex);
//                 this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
//                 if (this->stop && this->tasks.empty())
//                     return;
//                 task = std::move(this->tasks.front());
//                 this->tasks.pop();
//             }

//             task();
//         }
//     }

//    public:
//     // 获取线程池的单例实例
//     static ThreadPool& getInstance(unsigned int threads) {
//         static ThreadPool instance(threads);
//         return instance;
//     }

//     // 添加任务到线程池
//     void addTask(std::function<void()> task) {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         if (stop) throw std::runtime_error("Adding task to stopped ThreadPool");
//         tasks.push(task);
//         condition.notify_one();
//     }

//     // 停止所有线程
//     void stopAll() {
//         {
//             std::unique_lock<std::mutex> lock(queue_mutex);
//             stop = true;
//         }
//         condition.notify_all();
//         for (std::thread& worker : workers) {
//             if (worker.joinable()) worker.join();
//         }
//     }

//     // 析构函数，停止线程池
//     ~ThreadPool() {
//         stopAll();
//     }
// };

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool {
   private:
    static ThreadPool* instance;
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mtx;
    std::condition_variable cond;
    bool isClosed;
    ThreadPool(unsigned int threads) {
        for (auto i = 0; i < threads; ++i) {
            workers.emplace_back(&ThreadPool::workerTask, this);
        }
    }
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    void workerTask() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->queue_mtx);
                this->cond.wait(lock, [this] { return this->isClosed || !this->tasks.empty(); });
                if (this->isClosed && this->tasks.empty())
                    return;
                task = std::move(this->tasks.front());
                this->tasks.pop();
            }
            task();
        }
    }

   public:
    static ThreadPool& getInstance(unsigned int threads) {
        std::mutex mtx;
        if (instance == nullptr) {
            std::unique_lock<std::mutex> locker(mtx);
            if (instance == nullptr) {
                instance = new ThreadPool(threads);
            }
        }
        return *instance;
    }
    void addTask(std::function<void()> task) {
        std::unique_lock<std::mutex> lock(queue_mtx);
        if (isClosed) throw std::runtime_error("add task to stopped threadPool");
        tasks.push(task);
        cond.notify_one();
    }
    void stopAll() {
        {
            std::unique_lock<std::mutex> lock(queue_mtx);
            isClosed = true;
        }
        cond.notify_all();
        for (std::thread& worker : workers) {
            if (worker.joinable()) worker.join();
        }
    }

    ~ThreadPool() {
        stopAll();
    }
};

ThreadPool* ThreadPool::instance = nullptr;