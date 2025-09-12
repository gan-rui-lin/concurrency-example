#include <iostream>
#include <thread>

void hello() {
    std::cout << "Hello, World from thread!" << std::endl;
}

int main() {
    std::thread t(hello);
    // thread::join
    // Blocks the current thread until the thread identified by *this finishes its execution.
    // 在这个例子中，如果 t 线程还在运行，主线程会等待它完成。
    t.join();
    // 如果不等待线程汇入 (detach)，就必须保证线程结束之前，访问数据的有效性

    auto bad_func = [](int* start){
        for (int i = 0; i < 1000000; i++) {
            std::cout << *start + i << std::endl;
        }
    };

    int local_start = 100;
    std::thread t2(bad_func, &local_start);
    t2.detach();
    local_start = 200; // 这里修改了 local_start 的值，可能会导致线程 t2 访问到错误的值
    std::cout << "主线程准备结束！local_start的值不再可靠" << std::endl;

    return 0;
}