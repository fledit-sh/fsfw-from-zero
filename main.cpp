// Necessary header files for input output functions
#include "fsfw/objectmanager.h"
#include "fsfw/tasks/TaskFactory.h"

#include "mission/ObjectFactory.h"
#include "mission/WebcamDeviceHandler.h"
#include "mission/tmtc/TmtcInfrastructure.h"
#include "mission/tmtc/WebcamCommandingService.h"
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

    auto* webcamHandler = objectManager->get<WebcamDeviceHandler>(webcam::objectIdWebcamHandler);
    auto* webcamService =
        objectManager->get<webcam::WebcamCommandingService>(webcam::objectIdWebcamCommandingService);
    auto* telemetrySink =
        objectManager->get<webcam::StubTelemetrySink>(webcam::objectIdWebcamTelemetrySink);
    auto* verificationSink = objectManager->get<webcam::StubVerificationReceiver>(
        webcam::objectIdWebcamVerificationSink);
    auto* pusDistributor =
        objectManager->get<webcam::StubPusDistributor>(webcam::objectIdWebcamTcDistributor);
    (void)webcamHandler;
    (void)webcamService;

    auto* taskFactory = TaskFactory::instance();
    auto priority = 0;
    PeriodicTaskIF* webcamTask = taskFactory->createPeriodicTask(
        "WEBCAM_TASK", priority, PeriodicTaskIF::MINIMUM_STACK_SIZE, 1.0, nullptr);
    webcamTask->addComponent(webcam::objectIdWebcamHandler);
    webcamTask->startTask();

    PeriodicTaskIF* tmtcTask = taskFactory->createPeriodicTask(
        "TMTC_TASK", priority, PeriodicTaskIF::MINIMUM_STACK_SIZE, 1.0, nullptr);
    if (webcamService != nullptr) {
        tmtcTask->addComponent(webcam::objectIdWebcamCommandingService);
    }
    if (telemetrySink != nullptr) {
        tmtcTask->addComponent(webcam::objectIdWebcamTelemetrySink);
    }
    if (verificationSink != nullptr) {
        tmtcTask->addComponent(webcam::objectIdWebcamVerificationSink);
    }
    tmtcTask->startTask();

    using namespace std::chrono_literals;
    bool commandsQueued = false;
    while (true) {
        if (!commandsQueued && pusDistributor != nullptr) {
            double newFrameRate = 24.0;
            pusDistributor->sendCommand(
                static_cast<uint8_t>(webcam::WebcamCommandingService::Subservice::COMMAND_SET_FRAME_RATE),
                reinterpret_cast<const uint8_t*>(&newFrameRate), sizeof(newFrameRate));
            pusDistributor->sendCommand(
                static_cast<uint8_t>(webcam::WebcamCommandingService::Subservice::COMMAND_GET_FRAME_RATE));
            pusDistributor->sendCommand(
                static_cast<uint8_t>(webcam::WebcamCommandingService::Subservice::PARAMETER_DUMP));
            commandsQueued = true;
        }
        std::this_thread::sleep_for(1s);
    }
    return 0;
}