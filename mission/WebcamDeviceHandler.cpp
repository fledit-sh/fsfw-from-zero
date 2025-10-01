#include "WebcamDeviceHandler.h"

#include "fsfw/datapool/PoolEntry.h"
#include "fsfw/objectmanager/frameworkObjects.h"
#include "fsfw/serviceinterface/ServiceInterface.h"

#include <iomanip>
#include <iostream>

namespace {
constexpr const char* LOG_PREFIX = "WebcamHandlerStub";
}

WebcamDeviceHandler::WebcamDeviceHandler(object_id_t objectId)
    : DeviceHandlerBase(objectId, objects::NO_OBJECT, nullptr) {
  setStartUpImmediately();
}

void WebcamDeviceHandler::doStartUp() {
  sif::info << LOG_PREFIX << ": transitioning to MODE_ON stub" << std::endl;
  setMode(MODE_ON);
}

void WebcamDeviceHandler::doShutDown() {
  sif::info << LOG_PREFIX << ": shutting down" << std::endl;
  setMode(MODE_OFF);
}

ReturnValue_t WebcamDeviceHandler::buildNormalDeviceCommand(DeviceCommandId_t deviceCommand) {
  sif::info << LOG_PREFIX << ": buildNormalDeviceCommand called for ID 0x" << std::hex
            << static_cast<unsigned int>(deviceCommand) << std::dec << std::endl;
  return returnvalue::OK;
}

ReturnValue_t WebcamDeviceHandler::buildTransitionDeviceCommand(DeviceCommandId_t deviceCommand) {
  sif::info << LOG_PREFIX << ": buildTransitionDeviceCommand for ID 0x" << std::hex
            << static_cast<unsigned int>(deviceCommand) << std::dec << std::endl;
  return returnvalue::OK;
}

ReturnValue_t WebcamDeviceHandler::buildCommandFromCommand(DeviceCommandId_t deviceCommand,
                                                         const uint8_t* commandData,
                                                         size_t commandDataLen) {
  sif::info << LOG_PREFIX << ": buildCommandFromCommand (TC) for ID 0x" << std::hex
            << static_cast<unsigned int>(deviceCommand) << std::dec << ", payload length "
            << commandDataLen << std::endl;
  return DeviceHandlerIF::COMMAND_NOT_IMPLEMENTED;
}

ReturnValue_t WebcamDeviceHandler::scanForReply(const uint8_t* start, size_t len) {
  sif::info << LOG_PREFIX << ": scanForReply stub, len=" << len << std::endl;
  return returnvalue::OK;
}

ReturnValue_t WebcamDeviceHandler::interpretDeviceReply(DeviceCommandId_t deviceCommand,
                                                       const uint8_t* reply, size_t replyLen) {
  sif::info << LOG_PREFIX << ": interpretDeviceReply stub for ID 0x" << std::hex
            << static_cast<unsigned int>(deviceCommand) << std::dec << ", reply length " << replyLen
            << std::endl;
  return returnvalue::OK;
}

void WebcamDeviceHandler::fillCommandAndReplyMap() {
  insertInCommandMap(CAPTURE_FRAME);
  insertInCommandMap(SET_EXPOSURE);
  insertInCommandMap(DUMP_STATUS);
}

void WebcamDeviceHandler::modeChanged() {
  sif::info << LOG_PREFIX << ": modeChanged -> base mode " << static_cast<int>(getMode())
            << " submode " << static_cast<int>(getSubmode()) << std::endl;
}

ReturnValue_t WebcamDeviceHandler::initializeLocalDataPool(localpool::DataPool& localDataPoolMap) {
  // Reserve placeholder slots so telemetry packets can be hooked up later.
  localDataPoolMap.emplace(PARAM_EXPOSURE_TIME, new PoolEntry<uint32_t>(&exposureTimeMs, 1, true));
  localDataPoolMap.emplace(PARAM_FRAME_INTERVAL, new PoolEntry<uint32_t>(&frameIntervalMs, 1, true));
  return returnvalue::OK;
}

ReturnValue_t WebcamDeviceHandler::getParameter([[maybe_unused]] uint8_t domainId,
                                              uint8_t uniqueIdentifier,
                                              ParameterWrapper* parameterWrapper,
                                              const ParameterWrapper* newValues,
                                              [[maybe_unused]] uint16_t startAtIndex) {
  if (parameterWrapper == nullptr) {
    return returnvalue::FAILED;
  }
  switch (uniqueIdentifier) {
    case PARAM_EXPOSURE_TIME:
      parameterWrapper->set(exposureTimeMs);
      break;
    case PARAM_FRAME_INTERVAL:
      parameterWrapper->set(frameIntervalMs);
      break;
    default:
      return HasParametersIF::INVALID_IDENTIFIER_ID;
  }

  if (newValues != nullptr) {
    uint32_t updatedValue = 0;
    if (newValues->getElement(&updatedValue) == returnvalue::OK) {
      if (uniqueIdentifier == PARAM_EXPOSURE_TIME) {
        exposureTimeMs = updatedValue;
      } else if (uniqueIdentifier == PARAM_FRAME_INTERVAL) {
        frameIntervalMs = updatedValue;
      }
    }
  }
  return returnvalue::OK;
}