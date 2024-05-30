#include <iostream>
#include <map>
#include <memory>
#include <string>

// 一个简单的单例基类
class Singleton3 {
   protected:
    Singleton3() {
        std::cout << "Singleton instance created." << std::endl;
    }

   public:
    virtual ~Singleton3() {
        std::cout << "Singleton instance destroyed." << std::endl;
    }

    virtual void doSomething() = 0;  // 纯虚函数，由子类实现
};

// 单例工厂类
class SingletonFactory {
   private:
    static std::map<std::string, std::unique_ptr<Singleton3>> instances;

    // 私有构造函数，防止创建实例
    SingletonFactory() = default;

   public:
    // 注册单例类型
    template <typename T>
    static void registerType(const std::string& type) {
        instances[type] = std::make_unique<T>();
    }

    // 获取单例实例
    static Singleton3* getInstance(const std::string& type) {
        if (instances.find(type) == instances.end()) {
            throw std::runtime_error("Singleton type not registered.");
        }
        return instances[type].get();
    }
};

// 静态成员初始化
std::map<std::string, std::unique_ptr<Singleton3>> SingletonFactory::instances;

// 一个具体的单例实现
class ConcreteSingleton : public Singleton3 {
   public:
    void doSomething() override {
        std::cout << "ConcreteSingleton doing something." << std::endl;
    }
};
