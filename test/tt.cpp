#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool{
    private:
    static ThreadPool* instance;
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mtx;
    std::condition_variable cond;
    bool isClosed;
    ThreadPool(unsigned int threads){
        for(auto i=0;i<threads;++i){
            workers.emplace_back(&ThreadPool::workerTask,this);
        }
    }
    ThreadPool(const ThreadPool&)=delete;
    ThreadPool& operator=(const ThreadPool&)=delete;
    void workerTask(){
        while (true)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->queue_mtx);
                this->cond.wait(lock,[this]{return this->isClosed||!this->tasks.empty();});
                if(this->isClosed&&this->tasks.empty())
                return;
                task=std::move(this->tasks.front());
                this->tasks.pop();
            }
            task();
        }
        
    }
    public:
    static ThreadPool& getInstance(unsigned int threads){
        std::mutex mtx;
        if(instance==nullptr){
            std::unique_lock<std::mutex> locker(mtx);
            if(instance==nullptr){
                instance=new ThreadPool(threads);
            }
        }
    }
    void addTask(std::function<void()> task){
        std::unique_lock<std::mutex> lock(queue_mtx);
        if(isClosed) throw std::runtime_error("add task to stopped threadPool");
        tasks.push(task);
        cond.notify_one();
    }
    void stopAll(){
        {
            std::unique_lock<std::mutex> lock(queue_mtx);
            isClosed=true;
        }
        cond.notify_all();
        for(std::thread& worker:workers){
            if(worker.joinable()) worker.join();
        }
    }

    ~ThreadPool(){
        stopAll();
    }
};

ThreadPool* ThreadPool::instance=nullptr;