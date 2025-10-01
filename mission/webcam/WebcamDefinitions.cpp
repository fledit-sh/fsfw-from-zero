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