//
// Created by noel on 01.10.25.
//

#ifndef FSFW_FROM_ZERO_WEBCAMDEFINITIONS_H
#define FSFW_FROM_ZERO_WEBCAMDEFINITIONS_H


#pragma once

#include <fsfw/devicehandlers/DeviceHandlerIF.h>
#include <fsfw/objectmanager/SystemObjectIF.h>

#include <cstdint>

namespace webcam {

    enum class CommandId : DeviceCommandId_t {
        commandTakeSnapshot = 0x01,
        commandSetFrameRate = 0x02,
        commandGetFrameRate = 0x03,
    };

    const char *commandIdToString(CommandId command);

    enum class ParameterId : uint8_t {
        parameterFrameRate = 0x01,
    };

    const char *parameterIdToString(ParameterId parameter);

    inline constexpr object_id_t objectIdWebcamHandler = static_cast<object_id_t>(0x57000001);
    inline constexpr object_id_t objectIdWebcamCookie = static_cast<object_id_t>(0x57000002);
    inline constexpr object_id_t objectIdWebcamComIF = static_cast<object_id_t>(0x57000003);
    inline constexpr object_id_t objectIdWebcamCommandingService = static_cast<object_id_t>(0x57000010);
    inline constexpr object_id_t objectIdWebcamTcDistributor = static_cast<object_id_t>(0x57000011);
    inline constexpr object_id_t objectIdWebcamTelemetrySink = static_cast<object_id_t>(0x57000012);
    inline constexpr object_id_t objectIdWebcamVerificationSink = static_cast<object_id_t>(0x57000013);

}  // namespace webcam
#endif //FSFW_FROM_ZERO_WEBCAMDEFINITIONS_H
