// 在一些实际情景（如 DNS 缓存）中，读操作远多于写操作，使用读写锁可以提升并发性能。
// 读写锁允许多个线程同时读取共享数据，但在写操作时会独占锁，阻止其他读写操作。
// shared_mutex 具有 多个线程共享同一读互斥的所有权(注意是所有权，可能并不持有)、只有一个线程可以拥有互斥锁 的特性。
// 只有当互斥锁没有被任何线程获取时，多个线程才能获取共享锁。
// 代码参考：
// https://en.cppreference.com/w/cpp/thread/shared_mutex.html

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <syncstream>
 
class ThreadSafeCounter
{
public:
    ThreadSafeCounter() = default;
 
    // Multiple threads/readers can read the counter's value at the same time.
    unsigned int get() const
    {
        // 使用 shared_lock 共享地获取 mutex_
        std::shared_lock lock(mutex_);
        return value_;
    }
 
    // Only one thread/writer can increment/write the counter's value.
    void increment()
    {
        // 使用 unique_lock 互斥地获取 mutex_
        std::unique_lock lock(mutex_);
        ++value_;
    }
 
    // Only one thread/writer can reset/write the counter's value.
    void reset()
    {
        std::unique_lock lock(mutex_);
        value_ = 0;
    }
 
private:
    // 允许在 const 成员函数中修改 mutex_
    mutable std::shared_mutex mutex_;
    unsigned int value_{};
};
 
int main()
{
    ThreadSafeCounter counter;
 
    // 启动两个线程同时增加计数器的值，并打印当前值
    auto increment_and_print = [&counter]()
    {
        for (int i{}; i != 3; ++i)
        {
            counter.increment();
            {
                // c++20 引入，保证单个输出行的完整性
                std::osyncstream synced_out(std::cout);
                synced_out << std::this_thread::get_id() << ' ' << counter.get() << '\n';
            }
        }
    };
 
    std::thread thread1(increment_and_print);
    std::thread thread2(increment_and_print);
 
    thread1.join();
    thread2.join();
}