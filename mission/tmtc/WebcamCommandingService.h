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

#include <fsfw/devicehandlers/DeviceHandlerIF.h>
#include <fsfw/objectmanager/SystemObject.h>
#include <fsfw/parameters/ParameterMessage.h>
#include <fsfw/tmtcservices/CommandingServiceBase.h>

#include <cstddef>

#include "mission/webcam/WebcamDefinitions.h"

class CommandMessage;

namespace webcam {

    class WebcamCommandingService : public CommandingServiceBase {
    public:
        static constexpr uint16_t APID = 0x01;
        static constexpr uint8_t SERVICE_ID = 200;

        enum class Subservice : uint8_t {
            COMMAND_TAKE_SNAPSHOT = 1,
            COMMAND_SET_FRAME_RATE = 2,
            COMMAND_GET_FRAME_RATE = 3,
            PARAMETER_DUMP = 4,
            TM_PARAMETER_DUMP = 130,
            TM_COMMAND_DATA_REPLY = 131,
          };

        explicit WebcamCommandingService(object_id_t objectId, VerificationReporterIF* reporter = nullptr);

    protected:
        ReturnValue_t isValidSubservice(uint8_t subservice) override;
        ReturnValue_t getMessageQueueAndObject(uint8_t subservice, const uint8_t* tcData, size_t tcDataLen,
                                               MessageQueueId_t* id, object_id_t* objectId) override;
        ReturnValue_t prepareCommand(CommandMessage* message, uint8_t subservice, const uint8_t* tcData,
                                     size_t tcDataLen, uint32_t* state, object_id_t objectId) override;
        ReturnValue_t handleReply(const CommandMessage* reply, Command_t previousCommand, uint32_t* state,
                                  CommandMessage* optionalNextCommand, object_id_t objectId,
                                  bool* isStep) override;

    private:
        ReturnValue_t prepareDeviceCommand(CommandMessage* message, ::webcam::CommandId command,
                                           const uint8_t* tcData, size_t tcDataLen);
        ReturnValue_t prepareParameterDump(CommandMessage* message, const uint8_t* tcData, size_t tcDataLen);
        ReturnValue_t handleActionReply(const CommandMessage* reply, bool* isStep);
        ReturnValue_t handleParameterReply(const CommandMessage* reply, object_id_t objectId);
    };

}  // namespace webcam