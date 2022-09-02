#include <iostream>

#include "fsfw/serviceinterface.h"
#include "FSFWConfig.h"
#include "fsfw/objectmanager/SystemObject.h"
#include "fsfw/tasks/ExecutableObjectIF.h"
#include "fsfw/tasks/TaskFactory.h"
#include <thread>
#include <chrono>
#include <functional>

using namespace std;
using namespace returnvalue;

#if FSFW_CPP_OSTREAM_ENABLED == 1
ServiceInterfaceStream sif::debug("DEBUG", false);
ServiceInterfaceStream sif::info("INFO", false);
ServiceInterfaceStream sif::warning("WARNING", false);
ServiceInterfaceStream sif::error("ERROR", false, true, true);
#endif

class MyExecutable: public SystemObject, public ExecutableObjectIF {
  public:
    MyExecutable(object_id_t objectId): SystemObject(objectId) {}

    static int myTaskEntryPoint(MyExecutable& self) {
        using namespace std::chrono_literals;
        while (true) {
            self.performOperation(0);
            this_thread::sleep_for(1000ms);
        }
    }
    /**
     * @brief	The performOperation method is executed in a task.
     * @details	There are no restrictions for calls within this method, so any
     * 			other member of the class can be used.
     * @return	Currently, the return value is ignored.
     */
    ReturnValue_t performOperation(uint8_t operationCode) override {
        cout << "hello world from MyExecutable" << endl;
        return OK;
    }

    ReturnValue_t initialize() override {
        cout << "MyExecutable custom init" << endl;
        return OK;
    }

  private:
};

static const object_id_t MY_OBJ = 1;

void objFactory(void* args);

int main() {
    using namespace std::chrono_literals;
    cout << "hello world!" << endl;
    auto *objMan = ObjectManager::instance();
    objMan->setObjectFactoryFunction(&objFactory, nullptr);
    objMan->initialize();

    PeriodicTaskIF* task = TaskFactory::instance()->createPeriodicTask("MY_EXECUTABLE", 80, PeriodicTaskIF::MINIMUM_STACK_SIZE, 1.0, nullptr);
    task->addComponent(MY_OBJ);
    task->startTask();
    while (true) {
        this_thread::sleep_for(5000ms);
    }
}

void objFactory(void* args) {
    new MyExecutable(MY_OBJ);
}
