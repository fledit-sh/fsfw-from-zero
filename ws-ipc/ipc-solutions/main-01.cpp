#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

std::mutex SHARED_LOCK;
static uint32_t SHARED_COUNTER = 0;

void task0() {
    using namespace std::chrono_literals;
    while (true) {
        SHARED_LOCK.lock();
        cout << "Task0: Shared counter after increment" << ++SHARED_COUNTER << endl;    
        SHARED_LOCK.unlock();
        this_thread::sleep_for(1000ms);
    }
}

void task1() {
    using namespace std::chrono_literals;
    while(true) {
        {
            std::lock_guard lg(SHARED_LOCK);
            cout << "Task1: Shared counter after increment" << ++SHARED_COUNTER << endl;    
        }
        this_thread::sleep_for(1000ms);
    }
}

int main() {
    std::thread t0(task0);
    std::thread t1(task1);
    t0.join();
    t1.join();
}
