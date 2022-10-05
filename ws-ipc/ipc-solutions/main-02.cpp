#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <optional>

using namespace std;

std::mutex QUEUE_MUTEX;
std::queue<int> INT_QUEUE;

static uint32_t SENT_INTS = 0;

void intSender() {
    using namespace std::chrono_literals;
    while(true) {
        QUEUE_MUTEX.lock();
        INT_QUEUE.push(++SENT_INTS);
        QUEUE_MUTEX.unlock();
        cout << "intSender: Sent value " << SENT_INTS << endl;
        this_thread::sleep_for(1000ms);
    }
}

void intReceiver() {
    using namespace std::chrono_literals;
    while(true) {
        std::optional<int> optInt;
        QUEUE_MUTEX.lock();
        if(not INT_QUEUE.empty()) {
            optInt = INT_QUEUE.back();
            INT_QUEUE.pop();
        }
        QUEUE_MUTEX.unlock();
        if (optInt.has_value()) {
            cout << "intReceiver: Received integer value " <<
                optInt.value() << endl;
        }
        this_thread::sleep_for(800ms);
    }
}
int main() {
    cout << "Hello World" << endl;
    std::thread t0(intSender);
    std::thread t1(intReceiver);
    t0.join();
    t1.join();
}
