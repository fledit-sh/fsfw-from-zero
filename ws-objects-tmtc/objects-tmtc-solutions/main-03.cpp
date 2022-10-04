#include "fsfw/objectmanager.h"
#include "fsfw/tasks/TaskFactory.h"

#include <thread>
#include <iostream>
#include <iomanip>

#ifdef PLATFORM_WIN
#include "fsfw/osal/windows/winTaskHelpers.h"
#endif

using namespace std;

enum ObjectIds {
    TEST_OBJECT = 0x10101010
};

class MyObject: public ExecutableObjectIF, public SystemObject {
public:
    MyObject(object_id_t objectId): SystemObject(objectId) {}
    ReturnValue_t performOperation(uint8_t opCode) override {
        cout << "MyObject::performOperation: Periodic handling" << endl;
        return returnvalue::OK;
    }
    ReturnValue_t initialize() override {
        cout << "MyObject::initialize: Custom init" << endl;
        return returnvalue::OK;
    }
};

int main() {
    new MyObject(ObjectIds::TEST_OBJECT);
    auto* objManager = ObjectManager::instance();
    objManager->initialize();
    auto* mySysObj = objManager->get<MyObject>(ObjectIds::TEST_OBJECT);
    cout << "Object ID: " << setfill('0') << hex << "0x" << setw(8) <<
        mySysObj->getObjectId() << endl;
    auto* taskFactory = TaskFactory::instance();
#ifdef PLATFORM_WIN
    auto prio = tasks::makeWinPriority();
#else
    auto prio = 0;
#endif
    PeriodicTaskIF* periodicTask = taskFactory->createPeriodicTask("TEST_TASK", prio, PeriodicTaskIF::MINIMUM_STACK_SIZE, 1.0, nullptr);
    periodicTask->addComponent(ObjectIds::TEST_OBJECT);
    periodicTask->startTask();
    while(true) {
        using namespace std::chrono_literals;
        this_thread::sleep_for(5000ms);
    }
    return 0;
}
