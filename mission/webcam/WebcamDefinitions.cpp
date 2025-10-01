//
// Created by noel on 01.10.25.
//

#include "WebcamDefinitions.h"

#include "WebcamDefinitions.h"

namespace webcam {

    const char *commandIdToString(CommandId command) {
        switch (command) {
            case CommandId::commandTakeSnapshot:
                return "commandTakeSnapshot";
            case CommandId::commandSetFrameRate:
                return "commandSetFrameRate";
            case CommandId::commandGetFrameRate:
                return "commandGetFrameRate";
        }
        return "commandUnknown";
    }

    const char *parameterIdToString(ParameterId parameter) {
        switch (parameter) {
            case ParameterId::parameterFrameRate:
                return "parameterFrameRate";
        }
        return "parameterUnknown";
    }

}  // namespace webcam