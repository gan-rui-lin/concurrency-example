#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool turnA = true; // true表示轮到A打印，false表示轮到B打印

void printA(){

    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return turnA; }); // 等待轮到A打印
        std::cout << "A" << std::endl;
        turnA = false;
        cv.notify_all(); // 唤醒所有 B 线程
    }
}

void printB(){
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return !turnA; }); // 等待轮到B打印
        std::cout << "B" << std::endl;
        turnA = true;
        cv.notify_all(); // 唤醒所有 A 线程
    }
}

int main() {
    std::thread t1(printA);
    std::thread t2(printB);
    t1.join();
    t2.join();
    return 0;
}
