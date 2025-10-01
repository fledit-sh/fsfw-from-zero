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

#pragma once

#include <fsfw/devicehandlers/CookieIF.h>
#include <fsfw/objectmanager/SystemObject.h>
#include "WebcamDefinitions.h"

#include <string>

class WebcamCookie : public CookieIF, public SystemObject {
public:
    WebcamCookie(std::string devicePath, double initialFrameRate);

    [[nodiscard]] const std::string &getDevicePath() const;
    [[nodiscard]] double getInitialFrameRate() const;

private:
    std::string devicePath;  // linux device paths. very clever.
    double initialFrameRate; // initial framerate
};
