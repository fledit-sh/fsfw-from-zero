#pragma once

#include <cstdint>

#include <fsfw/devicehandlers/DeviceHandlerIF.h>
#include "mission/webcam/WebcamDefinitions.h"

namespace messagetypes::mission::webcam {

    // Mission command to capture a single image from the webcam sensor.
    inline constexpr DeviceCommandId_t TAKE_SNAPSHOT = static_cast<DeviceCommandId_t>(webcam::CommandId::commandTakeSnapshot);
    inline constexpr DeviceCommandId_t SET_FRAME_RATE = static_cast<DeviceCommandId_t>(webcam::CommandId::commandSetFrameRate);
    inline constexpr DeviceCommandId_t GET_FRAME_RATE = static_cast<DeviceCommandId_t>(webcam::CommandId::commandGetFrameRate);
    inline constexpr uint8_t PARAM_FRAME_RATE = static_cast<uint8_t>(webcam::ParameterId::parameterFrameRate);

    [[nodiscard]] bool rawToCommand(DeviceCommandId_t rawId, webcam::CommandId &command);
    [[nodiscard]] DeviceCommandId_t commandToRaw(webcam::CommandId command);
    [[nodiscard]] bool rawToParameter(uint8_t rawId, webcam::ParameterId &parameter);
    [[nodiscard]] uint8_t parameterToRaw(webcam::ParameterId parameter);

}  // namespace messagetypes::mission::webcam
