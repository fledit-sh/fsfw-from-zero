//
// Created by noel on 01.10.25.
//

#ifndef FSFW_FROM_ZERO_WEBCAMDEFINITIONS_H
#define FSFW_FROM_ZERO_WEBCAMDEFINITIONS_H

#include "fsfw/devicehandlers/CookieIF.h"

/** Introducing object IDs **/
namespace objects {
    static constexpr std::uint32_t WEBCAM_COM_IF = 0x5000;
    static constexpr std::uint32_t WEBCAM_HANDLER = 0x5001;
}

struct WebcamConfig {
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    float framesPerSecond = 0.0f;
};


class WebcamCookie : public CookieIF {
    public:
        explicit WebcamCookie(const WebcamConfig& config) : config(config) {}
        const WebcamConfig& getConfig() const { return config; }
        void setConfig(const WebcamConfig& newConfig) { config = newConfig; }

    private:
        WebcamConfig config;
};



#endif //FSFW_FROM_ZERO_WEBCAMDEFINITIONS_H