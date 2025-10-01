// Necessary header files for input output functions
#include "fsfw/objectmanager.h"
#include "fsfw/tasks/TaskFactory.h"

#include "mission/WebcamDeviceHandler.h"
#include "mission/ObjectFactory.h"
#include "mission/webcam/WebcamDefinitions.h"

#include <chrono>
#include <thread>

using namespace std;

// main() function: where the execution of
// C++ program begins
int main() {
    auto* objectManager = ObjectManager::instance();
    ObjectFactory::createMissionObjects();
    objectManager->initialize();

    auto* webcamHandler = objectManager->get<WebcamDeviceHandler>(
        webcam::objectIdWebcamHandler);
    (void)webcamHandler;

    auto* taskFactory = TaskFactory::instance();
    auto priority = 0;
    PeriodicTaskIF* webcamTask = taskFactory->createPeriodicTask(
        "WEBCAM_TASK", priority, PeriodicTaskIF::MINIMUM_STACK_SIZE, 1.0, nullptr);
    webcamTask->addComponent(webcam::objectIdWebcamHandler);
    webcamTask->startTask();
    using namespace std::chrono_literals;
    while (true) {
        std::this_thread::sleep_for(5s);
    }
    return 0;
}