// 参考链接：
// https://veitchkyrie.github.io/2019/09/12/%E9%81%BF%E5%85%8D%E6%AD%BB%E9%94%81%E4%B9%8Bscoped_lock/

// 这里是使用 C++17 标准的 std::scoped_lock 来解决多个互斥锁获取时的死锁问题

// 一个线程先获取锁 A，然后获取锁 B
// 另一个线程先获取锁 B，然后获取锁 A
// 并且不使用 scoped_lock 来同时获取锁;

#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>

std::mutex mutex_a;
std::mutex mutex_b;

void thread_func1() {
    std::cout << "线程1在尝试获取锁A" << std::endl;
    std::lock_guard<std::mutex> lock_a(mutex_a);
    std::cout << "线程1已获取锁A，准备休眠" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // CPU 让步，增加死锁概率
    std::cout << "线程1在尝试获取锁B" << std::endl;
    std::lock_guard<std::mutex> lock_b(mutex_b);
    std::cout << "线程1获取了锁A和锁B" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "线程1释放了锁A和锁B" << std::endl;
}

void thread_func2() {
    std::cout << "线程2在尝试获取锁B" << std::endl;
    std::lock_guard<std::mutex> lock_b(mutex_b);

    std::cout << "线程2已获取锁B，准备休眠" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "线程2在尝试获取锁A" << std::endl;
    std::lock_guard<std::mutex> lock_a(mutex_a);
    std::cout << "线程2获取了锁A和锁B" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "线程2释放了锁A和锁B" << std::endl;
}

void thread_func1_scoped() {
    std::cout << "线程1在尝试获取锁A和锁B" << std::endl;
    std::scoped_lock lock(mutex_a, mutex_b); // 同时获取两个锁，避免死锁
    std::cout << "线程1获取了锁A和锁B" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "线程1释放了锁A和锁B" << std::endl;
}

void thread_func2_scoped() {
    std::cout << "线程2在尝试获取锁A和锁B" << std::endl;
    std::scoped_lock lock(mutex_a, mutex_b); // 同时获取两个锁，避免死锁
    std::cout << "线程2获取了锁A和锁B" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "线程2释放了锁A和锁B" << std::endl;
}

int main() {
    // 下面情形一定死锁
    // std::thread t1(thread_func1);
    // std::thread t2(thread_func2);
    // t1.join();
    // t2.join();

    std::thread t1(thread_func1_scoped);
    std::thread t2(thread_func2_scoped);
    t1.join();
    t2.join();
    return 0;
}