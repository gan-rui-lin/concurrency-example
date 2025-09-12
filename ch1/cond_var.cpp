// 条件变量会与多个事件或其他条件相关，并且一个或多个线程会等待条件的达成。当某些线程被终止时，为
// 了唤醒等待线程(允许等待线程继续执行)，终止线程将会向等待着的线程广播“条件达成”的信息。

// notify_one() 每次只能唤醒一个线程，就不会出现多线程去抢夺互斥量的情况。
// 而 notify_all() 可以唤醒所有的线程，但是最终能抢夺锁的只有一个线程，
// 对于这些没有抢到控制权的这个过程就叫做 虚假唤醒

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <iostream>

typedef int data_chunk;

std::mutex mut;
// 在两个线程间共享的数据队列
std::queue<data_chunk> data_queue;

std::condition_variable data_cond;

// Dummy implementation for demonstration
bool more_data_to_prepare() {
    static int count = 10;
    return count-- > 0;
}

// Dummy implementation for demonstration
data_chunk prepare_data() {
    static int value = 10;
    return value--;
}

void data_preparation_thread()
{
    while(more_data_to_prepare()) {
        data_chunk const data = prepare_data();
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        data_cond.notify_one(); // 通知等待的线程，在通知前,等待的线程会被阻塞(睡眠)而不会占用CPU时间
    }
}

void process(data_chunk data) {
    // 示例处理函数，实际应用中可替换为实际处理逻辑
    std::cout << "Processing data: " << data << std::endl;
}

void data_processing_thread()
{
    while(true) {
        data_chunk data;
        {
            // 等待中的线程必须在等待期间解锁互斥量，并对互斥量再次上锁
            // 所以这里使用 unique_lock 而不是 lock_guard，因为 lock_guard 不支持解锁
            std::unique_lock<std::mutex> lk(mut);
            // 让线程在条件不满足时​​主动放弃CPU进入睡眠​​，只在有可能满足条件时（被通知或发生伪唤醒）才醒来检查
            // lambda 表达式为 True 的时候返回并继续持有锁，否则阻塞该线程并放锁
            data_cond.wait(lk, []{return !data_queue.empty();});
            data = data_queue.front();
            data_queue.pop();
        }
        process(data);
        if (data == 1) break; // 假设1表示结束
    }
}

// main 函数，验证条件变量功能
int main() {
    std::thread producer(data_preparation_thread);
    std::thread consumer(data_processing_thread);
    producer.join();
    consumer.join();
    std::cout << "All data processed." << std::endl;
    return 0;
}