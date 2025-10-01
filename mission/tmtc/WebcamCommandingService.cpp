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

#include "WebcamCommandingService.h"

#include <cstring>

#include "FSFWConfig.h"

#include <fsfw/action/ActionMessage.h>
#include <fsfw/ipc/CommandMessage.h>
#include <fsfw/objectmanager/ObjectManager.h>
#include <fsfw/parameters/HasParametersIF.h>
#include <fsfw/parameters/ReceivesParameterMessagesIF.h>
#include <fsfw/pus/servicepackets/Service20Packets.h>
#include <fsfw/pus/servicepackets/Service8Packets.h>
#include <fsfw/serviceinterface/ServiceInterface.h>
#include <fsfw/storagemanager/StorageManagerIF.h>

#include "mission/messaging/MessageTypes.h"

namespace webcam {
namespace {
constexpr uint8_t WEBCAM_PARAMETER_DOMAIN = 0;
}

WebcamCommandingService::WebcamCommandingService(object_id_t objectId, VerificationReporterIF* reporter)
    : CommandingServiceBase(objectId, APID, SERVICE_ID, fsfwconfig::FSFW_CSB_FIFO_DEPTH, 60, 20, reporter) {}
ReturnValue_t WebcamCommandingService::initialize() {
  ReturnValue_t result = CommandingServiceBase::initialize();
  if (result != returnvalue::OK) {
    sif::printError(
        "WebcamCommandingService::initialize: Base initialization failed with code %u\n",
        static_cast<unsigned int>(result));
    return result;
  }

  if (ipcStore == nullptr) {
    sif::printError("WebcamCommandingService::initialize: IPC store unavailable\n");
    return returnvalue::FAILED;
  }

  return returnvalue::OK;
}

ReturnValue_t WebcamCommandingService::isValidSubservice(uint8_t subservice) {
  switch (static_cast<Subservice>(subservice)) {
    case Subservice::COMMAND_TAKE_SNAPSHOT:
    case Subservice::COMMAND_SET_FRAME_RATE:
    case Subservice::COMMAND_GET_FRAME_RATE:
    case Subservice::PARAMETER_DUMP:
      return returnvalue::OK;
    default:
      return AcceptsTelecommandsIF::INVALID_SUBSERVICE;
  }
}

ReturnValue_t WebcamCommandingService::getMessageQueueAndObject(uint8_t subservice, const uint8_t* tcData,
                                                                size_t tcDataLen, MessageQueueId_t* id,
                                                                object_id_t* objectId) {
  *objectId = webcam::objectIdWebcamHandler;
  switch (static_cast<Subservice>(subservice)) {
    case Subservice::COMMAND_TAKE_SNAPSHOT:
    case Subservice::COMMAND_SET_FRAME_RATE:
    case Subservice::COMMAND_GET_FRAME_RATE: {
      auto* handler = ObjectManager::instance()->get<DeviceHandlerIF>(*objectId);
      if (handler == nullptr) {
        return CommandingServiceBase::INVALID_OBJECT;
      }
      *id = handler->getCommandQueue();
      return returnvalue::OK;
    }
    case Subservice::PARAMETER_DUMP: {
      auto* receiver = ObjectManager::instance()->get<ReceivesParameterMessagesIF>(*objectId);
      if (receiver == nullptr) {
        return CommandingServiceBase::INVALID_OBJECT;
      }
      *id = receiver->getCommandQueue();
      return returnvalue::OK;
    }
    default:
      return CommandingServiceBase::INVALID_TC;
  }
}

ReturnValue_t WebcamCommandingService::prepareCommand(CommandMessage* message, uint8_t subservice,
                                                      const uint8_t* tcData, size_t tcDataLen, uint32_t* state,
                                                      object_id_t) {
  switch (static_cast<Subservice>(subservice)) {
    case Subservice::COMMAND_TAKE_SNAPSHOT:
      return prepareDeviceCommand(message, ::webcam::CommandId::commandTakeSnapshot, tcData, tcDataLen);
    case Subservice::COMMAND_SET_FRAME_RATE:
      return prepareDeviceCommand(message, ::webcam::CommandId::commandSetFrameRate, tcData, tcDataLen);
    case Subservice::COMMAND_GET_FRAME_RATE:
      return prepareDeviceCommand(message, ::webcam::CommandId::commandGetFrameRate, tcData, tcDataLen);
    case Subservice::PARAMETER_DUMP:
      return prepareParameterDump(message, tcData, tcDataLen);
    default:
      return CommandingServiceBase::INVALID_SUBSERVICE;
  }
}

ReturnValue_t WebcamCommandingService::handleReply(const CommandMessage* reply, Command_t, uint32_t*,
                                                   CommandMessage*, object_id_t objectId, bool* isStep) {
  switch (reply->getMessageType()) {
    case messagetypes::ACTION:
      return handleActionReply(reply, isStep);
    case messagetypes::PARAMETER:
      return handleParameterReply(reply, objectId);
    default:
      return CommandingServiceBase::INVALID_REPLY;
  }
}

ReturnValue_t WebcamCommandingService::prepareDeviceCommand(CommandMessage* message,
                                                            ::webcam::CommandId command,
                                                            const uint8_t* tcData, size_t tcDataLen) {
  uint8_t parameterBuffer[sizeof(double)] = {};
  size_t parameterSize = 0;
  switch (command) {
    case ::webcam::CommandId::commandTakeSnapshot:
    case ::webcam::CommandId::commandGetFrameRate:
      if (tcDataLen != 0) {
        return CommandingServiceBase::INVALID_TC;
      }
      parameterSize = 0;
      break;
    case ::webcam::CommandId::commandSetFrameRate:
      if (tcDataLen != sizeof(double) || tcData == nullptr) {
        return CommandingServiceBase::INVALID_TC;
      }
      std::memcpy(parameterBuffer, tcData, sizeof(double));
      parameterSize = sizeof(double);
      break;
    default:
      return CommandingServiceBase::INVALID_TC;
  }
  if (ipcStore == nullptr) {
    sif::printError("WebcamCommandingService::prepareDeviceCommand: IPC store unavailable\n");
    return returnvalue::FAILED;
  }
  store_address_t storeId;
  uint8_t* storePtr = nullptr;
  ReturnValue_t result = ipcStore->getFreeElement(&storeId, parameterSize, &storePtr);
  if (result != returnvalue::OK) {
    return result;
  }
  if (parameterSize > 0) {
    std::memcpy(storePtr, parameterBuffer, parameterSize);
  }

  const auto rawCommand = messagetypes::mission::webcam::commandToRaw(command);
  const Command_t messageId = CommandMessageIF::makeCommandId(messagetypes::DEVICE_HANDLER_COMMAND,
                                                              static_cast<uint8_t>(rawCommand));
  message->setCommand(messageId);
  message->setParameter(storeId.raw);
  return returnvalue::OK;
}

ReturnValue_t WebcamCommandingService::prepareParameterDump(CommandMessage* message, const uint8_t* tcData,
                                                            size_t tcDataLen) {
  uint8_t parameterRaw = messagetypes::mission::webcam::PARAM_FRAME_RATE;
  if (tcDataLen > 0) {
    if (tcDataLen != 1 || tcData == nullptr) {
      return CommandingServiceBase::INVALID_TC;
    }
    parameterRaw = tcData[0];
  }
  ParameterId_t parameterId =
      HasParametersIF::getFullParameterId(WEBCAM_PARAMETER_DOMAIN, parameterRaw, /*linearIndex=*/0);
  ParameterMessage::setParameterDumpCommand(message, parameterId);
  return returnvalue::OK;
}

ReturnValue_t WebcamCommandingService::handleActionReply(const CommandMessage* reply, bool* isStep) {
  const Command_t replyId = reply->getCommand();
  switch (replyId) {
    case ActionMessage::COMPLETION_SUCCESS:
      return CommandingServiceBase::EXECUTION_COMPLETE;
    case ActionMessage::STEP_SUCCESS:
      *isStep = true;
      return returnvalue::OK;
    case ActionMessage::DATA_REPLY: {
      *isStep = true;
      store_address_t storeId = ActionMessage::getStoreId(reply);
      const uint8_t* data = nullptr;
      size_t size = 0;
      if (ipcStore == nullptr) {
        sif::printError("WebcamCommandingService::handleActionReply: IPC store unavailable\n");
        return returnvalue::FAILED;
      }
      ReturnValue_t result = ipcStore->getData(storeId, &data, &size);
      if (result != returnvalue::OK) {
        return result;
      }
      DataReply dataReply(webcam::objectIdWebcamHandler, ActionMessage::getActionId(reply), data,
                          static_cast<uint16_t>(size));
      result = sendTmPacket(static_cast<uint8_t>(Subservice::TM_COMMAND_DATA_REPLY), dataReply);
      ipcStore->deleteData(storeId);
      return result;
    }
    case ActionMessage::STEP_FAILED:
      *isStep = true;
      return ActionMessage::getReturnCode(reply);
    case ActionMessage::COMPLETION_FAILED:
      return ActionMessage::getReturnCode(reply);
    default:
      return CommandingServiceBase::INVALID_REPLY;
  }
}

ReturnValue_t WebcamCommandingService::handleParameterReply(const CommandMessage* reply, object_id_t objectId) {
  if (reply->getCommand() != ParameterMessage::REPLY_PARAMETER_DUMP) {
    return CommandingServiceBase::INVALID_REPLY;
  }
  store_address_t storeId = ParameterMessage::getStoreId(reply);
  const uint8_t* data = nullptr;
  size_t size = 0;
  if (ipcStore == nullptr) {
    sif::printError("WebcamCommandingService::handleParameterReply: IPC store unavailable\n");
    return returnvalue::FAILED;
  }
  ReturnValue_t result = ipcStore->getData(storeId, &data, &size);
  if (result != returnvalue::OK) {
    return result;
  }
  ParameterDumpReply tmReply(objectId, ParameterMessage::getParameterId(reply), data, size);
  result = sendTmPacket(static_cast<uint8_t>(Subservice::TM_PARAMETER_DUMP), tmReply);
  ipcStore->deleteData(storeId);
  return result;
}

}  // namespace webcam