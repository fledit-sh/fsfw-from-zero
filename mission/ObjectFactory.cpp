#include "ObjectFactory.h"

#include <memory>

#include "mission/WebcamDeviceHandler.h"
#include "mission/webcam/WebcamCookie.h"
#include "mission/webcam/WebcamDefinitions.h"

namespace {
    std::unique_ptr<WebcamCookie> webcamCookie;
    std::unique_ptr<WebcamDeviceHandler> webcamHandler;
}

void ObjectFactory::createMissionObjects() {
    if (webcamCookie == nullptr) {
        webcamCookie = std::make_unique<WebcamCookie>("/dev/video0", 30.0);
    }
    if (webcamHandler == nullptr) {
        webcamHandler = std::make_unique<WebcamDeviceHandler>(
            webcam::objectIdWebcamHandler, 0, webcamCookie.get(), nullptr, 20);
    }
}