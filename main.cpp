#include <iostream>
#include <thread>

using namespace std;

void mySimpleTask() {
    using namespace std::chrono_literals;
    while(true) {
        cout << "Hello World" << endl;
        this_thread::sleep_for(1000ms);
    }
}

int main() {
    std::thread thread(mySimpleTask);
    thread.join();
}
