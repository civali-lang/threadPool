#include <iostream>
#include <mutex>

class Singleton2 {
   private:
    // 私有构造函数，防止外部通过new创建实例
    Singleton2() {
        std::cout << "Singleton instance created2." << std::endl;
    }

    // 私有拷贝构造函数和赋值操作符，防止复制和赋值
    Singleton2(const Singleton2&) = delete;
    Singleton2& operator=(const Singleton2&) = delete;

   public:
    // 静态公有方法，返回类的唯一实例
    static Singleton2& getInstance() {
        static std::mutex mutex;  // 局部静态mutex，只在函数作用域内创建
        // 双重检查锁定模式，减少锁的粒度
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);  // 加锁
            if (instance == nullptr) {                // 再次检查，确保线程安全
                instance = new Singleton2();
            }
        }
        return *instance;
    }

    // 示例成员函数
    void doSomething() {
        std::cout << "Doing something2." << std::endl;
    }

    // 销毁实例的静态函数，用于释放资源
    static void destroyInstance() {
        delete instance;
        instance = nullptr;
    }

   private:
    // 静态私有实例指针，延迟实例化
    static Singleton2* instance;
};

// 静态实例指针的定义
Singleton2* Singleton2::instance = nullptr;
