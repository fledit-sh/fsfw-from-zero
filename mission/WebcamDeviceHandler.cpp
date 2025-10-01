#include "WebcamDeviceHandler.h"

#include <fsfw/devicehandlers/DeviceCommunicationIF.h>
#include <fsfw/returnvalues/HasReturnvaluesIF.h>
#include "webcam/WebcamDefinitions.h"
#include <fsfw/serviceinterface/ServiceInterface.h>

#include <cstring>
#include <iomanip>

/*
 * Trivial hacking: remove from cmake.txt to prevent failures!
 */

namespace {
  constexpr uint8_t STREAMING_ENABLED_REPLY_FLAG = 1;
  constexpr uint8_t STREAMING_DISABLED_REPLY_FLAG = 0;
}

WebcamDeviceHandler::WebcamDeviceHandler(object_id_t objectId, object_id_t deviceCommunication,
                                         CookieIF *comCookie, FailureIsolationBase *fdirInstance,
                                         size_t cmdQueueSize)
  // TODO: implement communication object IDs cookie instance and FDIR once handler is scheduled.
    : DeviceHandlerBase(objectId, deviceCommunication, comCookie, fdirInstance, cmdQueueSize),
      currentFrameRate(0.0), requestedFrameRate(0.0), snapshotRequested(false) {}

void WebcamDeviceHandler::doStartUp() {
  // This is called do transition to MODE_ON
  // TODO: implement calling logic by webcam_test application
  devicePowered = true;
  transitionCommandPending = true;
  setMode(MODE_ON);
}

void WebcamDeviceHandler::doShutDown() {
  // Release and destroy
  // TODO: BlackOps everything. Annihilation everywhere!
  devicePowered = false;
  transitionCommandPending = true;
  setMode(MODE_OFF);
}

uint32_t WebcamDeviceHandler::getTransitionDelayMs(Mode_t, Mode_t) {
  return 0;
}

ReturnValue_t WebcamDeviceHandler::buildTransitionDeviceCommand(DeviceCommandId_t *deviceCommand) {
  transitionCommandPending = false;
  if (deviceCommand != nullptr) {
    *deviceCommand = DeviceHandlerIF::NO_COMMAND_ID;
  }
  return DeviceHandlerBase::NOTHING_TO_SEND;
}


ReturnValue_t WebcamDeviceHandler::buildNormalDeviceCommand(DeviceCommandId_t *deviceCommand) {
  if (deviceCommand == nullptr) {
    return returnvalue::FAILED;
  }

  if (snapshotRequested) {
    snapshotRequested = false;
    snapshotInProgress = true;
    *deviceCommand = static_cast<DeviceCommandId_t>(webcam::CommandId::commandTakeSnapshot);
    prepareReply(*deviceCommand);
    return HasReturnvaluesIF::RETURN_OK;
  }

  if (frameRateCommandPending) {
    frameRateCommandPending = false;
    snapshotInProgress = false;
    currentFrameRate = requestedFrameRate;
    *deviceCommand = static_cast<DeviceCommandId_t>(webcam::CommandId::commandSetFrameRate);
    prepareReply(*deviceCommand);
    return HasReturnvaluesIF::RETURN_OK;
  }

  if (frameRateQueryPending) {
    frameRateQueryPending = false;
    snapshotInProgress = false;
    *deviceCommand = static_cast<DeviceCommandId_t>(webcam::CommandId::commandGetFrameRate);
    prepareReply(*deviceCommand);
    return HasReturnvaluesIF::RETURN_OK;
  }

  return DeviceHandlerBase::NOTHING_TO_SEND;
}

void WebcamDeviceHandler::fillCommandAndReplyMap() {
  // TODO: Mit konkreten command IDs befüllen
  using webcam::CommandId;
  insertInCommandAndReplyMap(static_cast<DeviceCommandId_t>(CommandId::commandTakeSnapshot), 0);
  insertInCommandAndReplyMap(static_cast<DeviceCommandId_t>(CommandId::commandSetFrameRate), 0);
  insertInCommandAndReplyMap(static_cast<DeviceCommandId_t>(CommandId::commandGetFrameRate), 0);
}

ReturnValue_t WebcamDeviceHandler::scanForReply(const uint8_t *, size_t,
                                                DeviceCommandId_t *foundId, size_t *foundLen) {
  // TODO: Parse incoming telemetry
  if (replyReady) {
    if (foundId != nullptr) {
      *foundId = fabricatedReplyId;
    }
    if (foundLen != nullptr) {
      *foundLen = 0;
    }
    if (fabricatedReplyId == static_cast<DeviceCommandId_t>(webcam::CommandId::commandTakeSnapshot)) {
      snapshotInProgress = false;
    }
    replyReady = false;
    return returnvalue::OK;
  }

  if (foundId != nullptr) {
    *foundId = DeviceHandlerIF::NO_COMMAND_ID;
  }
  if (foundLen != nullptr) {
    *foundLen = 0;
  }
  return DeviceCommunicationIF::NO_REPLY_RECEIVED;
}

ReturnValue_t WebcamDeviceHandler::interpretDeviceReply(DeviceCommandId_t id, const uint8_t *) {
  using webcam::CommandId;
  auto command = static_cast<CommandId>(id);

  switch (command) {
    case CommandId::commandTakeSnapshot:
      snapshotInProgress = false;
#if FSFW_CPP_OSTREAM_ENABLED == 1
      sif::info << "[Webcam] Snapshot request completed." << std::endl;
#else
      sif::printInfo("[Webcam] Snapshot request completed.\n");
#endif
      break;
    case CommandId::commandSetFrameRate:
#if FSFW_CPP_OSTREAM_ENABLED == 1
      sif::info << "[Webcam] Frame rate set to " << std::fixed << std::setprecision(2)
                << currentFrameRate << " fps." << std::defaultfloat << std::endl;
#else
      sif::printInfo("[Webcam] Frame rate set to %.2f fps.\n", currentFrameRate);
#endif
      break;
    case CommandId::commandGetFrameRate:
#if FSFW_CPP_OSTREAM_ENABLED == 1
      sif::info << "[Webcam] Current frame rate is " << std::fixed << std::setprecision(2)
                << currentFrameRate << " fps." << std::defaultfloat << std::endl;
#else
      sif::printInfo("[Webcam] Current frame rate is %.2f fps.\n", currentFrameRate);
#endif
      break;
    default:
#if FSFW_CPP_OSTREAM_ENABLED == 1
      sif::info << "[Webcam] Reply received for command 0x" << std::hex << id << std::dec << '.'
                << std::endl;
#else
      sif::printInfo("[Webcam] Reply received for command 0x%02x.\n", static_cast<unsigned int>(id));
#endif
      break;
  }

  fabricatedReplyId = DeviceHandlerIF::NO_COMMAND_ID;
  return returnvalue::OK;
}

ReturnValue_t WebcamDeviceHandler::getParameter(uint8_t domainId, uint8_t parameterId,
                                                ParameterWrapper *parameterWrapper,
                                                const ParameterWrapper *newValues,
                                                uint16_t startAtIndex) {

  return returnvalue::OK;
  using webcam::ParameterId;

  if (domainId != 0) {
    return DeviceHandlerBase::getParameter(domainId, parameterId, parameterWrapper, newValues,
                                           startAtIndex);
  }

  if (parameterId == static_cast<uint8_t>(ParameterId::parameterFrameRate)) {
    if (newValues == nullptr) {
      if (parameterWrapper == nullptr) {
        return returnvalue::FAILED;
      }
      parameterWrapper->set(currentFrameRate);
      return returnvalue::OK;
    }

    double newFrameRate = currentFrameRate;
    ReturnValue_t result = newValues->getElement(&newFrameRate);
    if (result != returnvalue::OK) {
      return result;
    }
    requestedFrameRate = newFrameRate;
    currentFrameRate = newFrameRate;
    return returnvalue::OK;
  }

  return DeviceHandlerBase::getParameter(domainId, parameterId, parameterWrapper, newValues,
                                         startAtIndex);
}
ReturnValue_t WebcamDeviceHandler::buildCommandFromCommand(DeviceCommandId_t deviceCommand,
                                                          const uint8_t *commandData,
                                                          size_t commandDataLen) {
  using webcam::CommandId;
  auto command = static_cast<CommandId>(deviceCommand);

  switch (command) {
    case CommandId::commandTakeSnapshot:
      snapshotRequested = true;
      return returnvalue::OK;
    case CommandId::commandSetFrameRate:
      if (commandData != nullptr && commandDataLen >= sizeof(double)) {
        double newFrameRate = 0.0;
        std::memcpy(&newFrameRate, commandData, sizeof(double));
        requestedFrameRate = newFrameRate;
      }
      frameRateCommandPending = true;
      return returnvalue::OK;
    case CommandId::commandGetFrameRate:
      frameRateQueryPending = true;
      return returnvalue::OK;
    default:
      return DeviceHandlerBase::COMMAND_NOT_SUPPORTED;
  }
}

void WebcamDeviceHandler::prepareReply(DeviceCommandId_t commandId) {
  fabricatedReplyId = commandId;
  replyReady = true;
  rawPacket = dummyCommandBuffer.data();
  rawPacketLen = 0;
}