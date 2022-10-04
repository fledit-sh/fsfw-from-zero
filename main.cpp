#include <iostream>
#include <thread>

#include "fsfw/platform.h"
#include "fsfw/tasks/ExecutableObjectIF.h"
#include "fsfw/tasks/PeriodicTaskIF.h"
#include "fsfw/tasks/TaskFactory.h"

#ifdef PLATFORM_WIN
#include "fsfw/osal/windows/winTaskHelpers.h"
#endif

using namespace std;

class MyExecutableObjectIF {
public:
    virtual ~MyExecutableObjectIF() = default;
    virtual void performOperation() = 0;
};

class MyExecutableObject0: public ExecutableObjectIF {
public:
    MyExecutableObject0() = default;

    ReturnValue_t performOperation(uint8_t opCode) override {
        cout << "Task 0" << endl;
        return returnvalue::OK;
    }
private:
};

class MyExecutableObject1: public ExecutableObjectIF  {
public:
    MyExecutableObject1() = default;

    ReturnValue_t performOperation(uint8_t opCode) override {
        cout << "Task 1" << endl;
        return returnvalue::OK;
    }
private:
};

class MyExecutableObject2: public ExecutableObjectIF  {
public:
    MyExecutableObject2() = default;

    ReturnValue_t performOperation(uint8_t opCode) override {
        cout << "Task 2" << endl;
        return returnvalue::OK;
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
    auto* factory = TaskFactory::instance();
#ifdef PLATFORM_WIN

#endif
    auto* periodicTask0 = factory->createPeriodicTask("TASK_0", 0, PeriodicTaskIF::MINIMUM_STACK_SIZE, 0.5, nullptr);
    auto* periodicTask1 = factory->createPeriodicTask("TASK_1", 0, PeriodicTaskIF::MINIMUM_STACK_SIZE, 1.0, nullptr);
    periodicTask0->addComponent(&myExecutableObject0);
    periodicTask0->addComponent(&myExecutableObject1);
    periodicTask1->addComponent(&myExecutableObject2);
    periodicTask0->startTask();
    periodicTask1->startTask();
    while(true) {
        this_thread::sleep_for(5000ms);
    }
}
