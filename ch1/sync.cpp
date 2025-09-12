#include <list>
#include <mutex>
#include <algorithm>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value) {
    // 局部变量 lock_guard ，确保互斥锁在函数结束时自动释放
    std::lock_guard<std::mutex> guard(some_mutex);
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find) {
    std::lock_guard<std::mutex> guard(some_mutex);
    return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}

// 验证被互斥锁正确保护
// 同一时刻只有一个线程能够读或者写

// 多线程测试
#include <thread>
#include <vector>
#include <iostream>

int main() {
    // 启动多个线程并发添加和查找
    std::vector<std::thread> threads;
    // 10个线程添加数据
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([i]{
            for (int j = 0; j < 1000; ++j) {
                add_to_list(i * 1000 + j);
            }
        });
    }
    
    // 5个线程查找数据
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]{
            int found = 0;
            for (int j = 0; j < 10000; ++j) {
                if (list_contains(j)) ++found;
            }
            std::cout << "Thread " << i << " found " << found << " values." << std::endl;
        });
    }
    for (auto& t : threads) t.join();
    // 主线程最终检查
    std::cout << "Final list size: " << some_list.size() << std::endl;
    return 0;
}