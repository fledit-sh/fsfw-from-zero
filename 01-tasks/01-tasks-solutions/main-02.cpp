#include <iostream>
#include <thread>

using namespace std;

class MyExecutableObject {
public:
    MyExecutableObject(uint32_t delayMs): delayMs(delayMs) {}

    static void executeTask(MyExecutableObject& self) {
        while(true) {
            self.performOperation();
            this_thread::sleep_for(std::chrono::milliseconds(self.delayMs));
        }
    }

    void performOperation() {
        cout << "Hello World" << endl;
    }
private:
    uint32_t delayMs;
};

int main() {
    MyExecutableObject myExecutableObject(1000);
    std::thread thread(
        MyExecutableObject::executeTask,
        std::reference_wrapper(myExecutableObject));
    thread.join();
    return 0;
}
