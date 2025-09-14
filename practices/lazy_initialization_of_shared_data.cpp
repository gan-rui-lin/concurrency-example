// 假设共享的数据源的构建代价很大，比如从数据库加载数据，或者从文件系统加载大量数据。
// 并且只是在需要的时候才进行初始化，以后则共享使用

#include <mutex>
#include <thread>
#include <iostream>
#include <memory>

std::shared_ptr<int> resource_ptr;
std::mutex mtx;

// 一个错误的 “双重检查” 实现
// 函数体的注释说明的是它为什么这么做
// 此处的注释说明其问题
// 1 处所示的读 ptr 操作和 2 处所示的写 ptr 操作之间没有同步，不能保证读取到正确的 ptr，3 处的行为不确定

void use_resource_bad() {
    /* 1 */
    if (!resource_ptr) { // 第一次检查，如果已经初始化，直接使用，避免加锁开销
        std::lock_guard<std::mutex> lock(mtx);
        if (!resource_ptr) { 
            // 第二次检查，考虑多个线程同时进入外面的 if，
            // 第一个线程持有锁并初始化；
            // 后进入的线程可能会错误的保持着 ptr 为空的想法(实际已经初始化)
            /* 2 */
            resource_ptr = std::make_shared<int>(42); // 假装代价很大的初始化
            std::cout << "Resource initialized (bad)" << std::endl;
        }
    }
    /* 3 */
    // 使用 resource_ptr
    std::cout << "Using resource: " << *resource_ptr << std::endl;
}


// 正确的实现，使用 std::call_once 和 std::once_flag
// std::call_once 确保某个初始化函数只被调用一次

// cout 的互斥锁
// 处理打印时的竞态
std::mutex cout_mutex;

// once_flag 用于标记某个初始化操作是否已经执行过
std::once_flag flag;

void use_resource_with_once_flag() {
    std::call_once(flag, [](){
        resource_ptr = std::make_shared<int>(42); // 假装代价很大的初始化
        std::cout << "Resource initialized (once_flag)" << std::endl;
    });
    // 使用 resource_ptr
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "Using resource: " << *resource_ptr << std::endl;
}

int main(){
    // 启动多个线程同时使用资源
    // 仍然仅初始化一次
    std::thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(use_resource_with_once_flag);
    }
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}