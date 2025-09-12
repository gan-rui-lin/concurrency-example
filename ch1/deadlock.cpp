#include <mutex>
class some_big_object {
    // ...
};

void swap(some_big_object& lhs, some_big_object& rhs) {
    // ...
}

class X{
    some_big_object some_detail;
    std::mutex m;
public:
    X(some_big_object const& sd) : some_detail(sd) {}

    friend void swap(X& lhs, X& rhs) {
        if (&lhs == &rhs) return;
        // // 1. 通过 std::lock 同时锁定两个互斥锁，避免死锁
        // std::lock(lhs.m, rhs.m);
        // // 2. 创建两个 lock_guard ，确保互斥锁在函数结束时自动释放(自动管理)
        // //    adopt_lock 指示锁管理对象​​接管​​一个​​已锁定​​的互斥量的所有权，​​而不重新上锁
        // std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
        // std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);
        // // 3. 交换数据成员
        // swap(lhs.some_detail, rhs.some_detail);

        // 使用 std::unique_lock
        // 在构造时​​先不要锁定​​互斥量，保持其未锁定状态。自己决定何时加锁。
        std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);
        std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);
        // 同时锁定两个互斥锁，避免死锁
        std::lock(lock_a, lock_b);
        swap(lhs.some_detail, rhs.some_detail);
    }
};