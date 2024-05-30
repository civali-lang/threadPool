#include <iostream>

class Singleton1 {
   private:
    // 私有构造函数，防止外部通过new创建实例
    Singleton1() {
        std::cout << "Singleton instance created1." << std::endl;
    }

    // 私有拷贝构造函数和赋值操作符，防止复制和赋值
    Singleton1(const Singleton1&) = delete;
    Singleton1& operator=(const Singleton1&) = delete;

   public:
    // 静态公有方法，返回类的唯一实例
    static Singleton1& getInstance() {
        return instance;  // 返回静态实例的引用
    }

    // 示例成员函数
    void doSomething() {
        std::cout << "Doing something1." << std::endl;
    }

   private:
    // 静态私有实例，在类加载时就创建
    static Singleton1 instance;
};

// 静态实例的定义
Singleton1 Singleton1::instance;