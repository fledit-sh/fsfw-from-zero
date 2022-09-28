#include <iostream>
#include <thread>

using namespace std;

class MyExecutableObjectIF {
public:
    virtual ~MyExecutableObjectIF() = default;
    virtual void performOperation() = 0;
};

class MyExecutableObject0: public MyExecutableObjectIF {
public:
    MyExecutableObject0() = default;

    void performOperation() override {
        cout << "Task 0" << endl;
    }
private:
};

class MyExecutableObject1: public MyExecutableObjectIF {
public:
    MyExecutableObject1() = default;

    void performOperation() override {
        cout << "Task 1" << endl;
    }
private:
};

class MyExecutableObject2: public MyExecutableObjectIF {
public:
    MyExecutableObject2() = default;

    void performOperation() override {
        cout << "Task 2" << endl;
    }
private:
};

class MyPeriodicTask {
public:
    MyPeriodicTask(MyExecutableObjectIF& executable, uint32_t taskFreqMs)
        : executable(executable), taskFreqMs(taskFreqMs) {}

    std::thread start() {
        return std::thread(
            MyPeriodicTask::executeTask,
            std::reference_wrapper(*this));
    }
private:
    static void executeTask(MyPeriodicTask& self) {
        while(true) {
            self.executable.performOperation();
            this_thread::sleep_for(std::chrono::milliseconds(self.taskFreqMs));
        }
    }
    MyExecutableObjectIF& executable;
    uint32_t taskFreqMs;
};

int main() {
    MyExecutableObject0 myExecutableObject0;
    MyExecutableObject1 myExecutableObject1;
    MyExecutableObject2 myExecutableObject2;
    MyPeriodicTask task0(myExecutableObject0, 1000);
    MyPeriodicTask task1(myExecutableObject1, 2000);
    MyPeriodicTask task2(myExecutableObject2, 5000);
    auto thread0 = task0.start();
    auto thread1 = task1.start();
    auto thread2 = task2.start();
    thread0.join();
    thread1.join();
    thread2.join();
    return 0;
}
