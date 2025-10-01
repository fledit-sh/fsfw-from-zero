//
// Created by noel on 01.10.25.
//
#pragma once

#include <fsfw/devicehandlers/CookieIF.h>

#include <string>

class WebcamCookie : public CookieIF {
public:
    WebcamCookie(std::string devicePath, double initialFrameRate);

    [[nodiscard]] const std::string &getDevicePath() const;
    [[nodiscard]] double getInitialFrameRate() const;

private:
    std::string devicePath;  // linux device paths. very clever.
    double initialFrameRate; // initial framerate
};


#endif //FSFW_FROM_ZERO_WEBCAMCOOKIE_H