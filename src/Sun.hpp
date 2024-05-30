
#include <mutex>
class Sun {
   private:
    // static Sun instance;
    static Sun* instance;
    Sun();
    Sun(const Sun&) = delete;
    Sun& operator=(const Sun&) = delete;

   public:
    static Sun& getInstance() {
        // return instance;
        static std::mutex mtx;
        if (instance == nullptr) {
            std::unique_lock<std::mutex> locker(mtx);
            if (instance == nullptr) {
                instance = new Sun();
            }
        }
        return *instance;
    }
};

// Sun Sun::instance;

Sun* Sun::instance = nullptr;