/**************************************************************
*  Project      : FSFWWebcamDemo
 *  Modul        : SW Development for Spacecraft
 *
 *  Autor        : Noel Ernsting Luz
 *  Co-Autor     : GPT-5 (KI-unterstützt)
 *  Erstellt am  : 2025-10-01
 *  Version      : 1.0
 *
 *  Hinweise     :
 *   - Teile des Codes wurden von GPT-5 generiert und
 *     von einem Menschen überprüft, angepasst und erweitert.
 *
 **************************************************************/


#include "WebcamCookie.h"

#include <utility>

WebcamCookie::WebcamCookie(std::string devicePath, double initialFrameRate)
: SystemObject(webcam::objectIdWebcamCookie),
  devicePath(std::move(devicePath)),
  initialFrameRate(initialFrameRate) {}
const std::string &WebcamCookie::getDevicePath() const { return devicePath; }

double WebcamCookie::getInitialFrameRate() const { return initialFrameRate; }