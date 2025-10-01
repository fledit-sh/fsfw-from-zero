// Necessary header files for input output functions
#include "fsfw/objectmanager.h"
#include "fsfw/tasks/TaskFactory.h"

#include "mission/WebcamDeviceHandler.h"
#include "mission/webcam/WebcamCookie.h"
#include "mission/webcam/WebcamDefinitions.h"

#include <chrono>
#include <memory>
#include <thread>

using namespace std;

// main() function: where the execution of
// C++ program begins
int main() {
    auto* objectManager = ObjectManager::instance();
    auto webcamCookie = std::make_unique<WebcamCookie>("/dev/video0", 30.0);
    auto* webcamHandler = new WebcamDeviceHandler(
        webcam::objectIdWebcamHandler, 0, webcamCookie.get(), nullptr, 20);
    (void)webcamHandler;

    objectManager->initialize();

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