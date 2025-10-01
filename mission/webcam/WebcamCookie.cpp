//
// Created by noel on 01.10.25.
//

#include "WebcamCookie.h"

#include <utility>

WebcamCookie::WebcamCookie(std::string devicePath, double initialFrameRate)
: SystemObject(webcam::objectIdWebcamCookie),
  devicePath(std::move(devicePath)),
  initialFrameRate(initialFrameRate) {}
const std::string &WebcamCookie::getDevicePath() const { return devicePath; }

double WebcamCookie::getInitialFrameRate() const { return initialFrameRate; }