//
// Created by noel on 01.10.25.
//

#include "mission/messaging/MessageTypes.h"

namespace messagetypes::mission::webcam {
    namespace {
        constexpr bool isValidCommand(::webcam::CommandId command) {
            switch (command) {
                case ::webcam::CommandId::commandTakeSnapshot:
                case ::webcam::CommandId::commandSetFrameRate:
                case ::webcam::CommandId::commandGetFrameRate:
                    return true;
                default:
                    return false;
            }
        }
        constexpr bool isValidParameter(::webcam::ParameterId parameter) {
            switch (parameter) {
                case ::webcam::ParameterId::parameterFrameRate:
                    return true;
                default:
                    return false;
            }
        }
    }  // namespace

    bool rawToCommand(DeviceCommandId_t rawId, ::webcam::CommandId &command) {
        auto candidate = static_cast<::webcam::CommandId>(rawId);
        if (!isValidCommand(candidate)) {
            return false;
        }
        command = candidate;
        return true;
    }

    DeviceCommandId_t commandToRaw(::webcam::CommandId command) {
        return static_cast<DeviceCommandId_t>(command);
    }

    bool rawToParameter(uint8_t rawId, ::webcam::ParameterId &parameter) {
        auto candidate = static_cast<::webcam::ParameterId>(rawId);
        if (!isValidParameter(candidate)) {
            return false;
        }
        parameter = candidate;
        return true;
    }

    uint8_t parameterToRaw(::webcam::ParameterId parameter) {
        return static_cast<uint8_t>(parameter);
    }
}