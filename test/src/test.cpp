#include "Singleton1.hpp"
#include "Singleton2.hpp"
#include "Singleton3.hpp"
#include "TreadPool.hpp"
void test1();
void test2();
void test3();
void testThreadPool(unsigned int n);

int main() {
    // test1();
    // test2();
    // test3();
    testThreadPool(30);
    return 0;
}
void test1() {
    // 获取单例对象并调用成员函数
    Singleton1& singleton = Singleton1::getInstance();
    singleton.doSomething();

    // 再次获取单例对象，应该得到相同的实例
    Singleton1& anotherSingleton = Singleton1::getInstance();
    anotherSingleton.doSomething();
    return;
}
void test2() {
    // 获取单例对象并调用成员函数
    Singleton2& singleton = Singleton2::getInstance();
    singleton.doSomething();

    // 再次获取单例对象，应该得到相同的实例
    Singleton2& anotherSingleton = Singleton2::getInstance();
    anotherSingleton.doSomething();

    // 程序结束前销毁单例实例
    Singleton2::destroyInstance();
    return;
}

void test3() {
    // 注册单例类型
    SingletonFactory::registerType<ConcreteSingleton>("ConcreteSingleton");

    // 获取并使用单例
    try {
        Singleton3* singleton = SingletonFactory::getInstance("ConcreteSingleton");
        singleton->doSomething();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}
// 测试函数，创建n个任务并交给线程池
void testThreadPool(unsigned int n) {
    unsigned int threads = std::thread::hardware_concurrency();  // 获取硬件支持的线程数
    ThreadPool& pool = ThreadPool::getInstance(threads);
    static std::mutex cout_mutex;
    // 创建并添加n个任务到线程池
    for (unsigned int i = 0; i < n; ++i) {
        pool.addTask([i]() {
            {
                std::lock_guard<std::mutex> guard(cout_mutex);  // 标准输出流只能由一个线程使用
                std::cout << "Task " << i << " is running" << std::endl;
            }
            // 模拟任务执行时间
            std::this_thread::sleep_for(std::chrono::seconds(1));
        });
    }

    // 等待所有任务完成
    std::this_thread::sleep_for(std::chrono::seconds(5));
}
