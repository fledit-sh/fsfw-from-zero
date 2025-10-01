#include "WebcamDeviceHandler.h"

#include <fsfw/devicehandlers/DeviceCommunicationIF.h>

WebcamDeviceHandler::WebcamDeviceHandler(object_id_t objectId, object_id_t deviceCommunication,
                                         CookieIF *comCookie, FailureIsolationBase *fdirInstance,
                                         size_t cmdQueueSize)
    : DeviceHandlerBase(objectId, deviceCommunication, comCookie, fdirInstance, cmdQueueSize) {
  // TODO: implement communication object IDs cookie instance and FDIR once handler is scheduled.
}

void WebcamDeviceHandler::doStartUp() {
  // This is called do transition to MODE_ON
  // TODO: implement calling logic by webcam_test application
}

void WebcamDeviceHandler::doShutDown() {
  // Release and destroy
  // TODO: BlackOps everything. Annihilation everywhere!
}

ReturnValue_t WebcamDeviceHandler::buildTransitionDeviceCommand(DeviceCommandId_t deviceCommand) {
  // Some more transitioning logic, we probably do not need.
  // TODO: translate transition command IDs?
  return returnvalue::OK;
}

ReturnValue_t WebcamDeviceHandler::buildNormalDeviceCommand(DeviceCommandId_t deviceCommand) {
  // TODO: Implement low level command frames once spefcified
  return returnvalue::OK;
}

void WebcamDeviceHandler::fillCommandAndReplyMap() {
  // TODO: Mit konkreten command IDs befüllen
}

ReturnValue_t WebcamDeviceHandler::scanForReply(const uint8_t *data, size_t len,
                                                DeviceCommandId_t *foundId, size_t *foundLen) {
  // TODO: Parse incoming telemetry
  if (foundId != nullptr) {
    *foundId = DeviceHandlerIF::NO_COMMAND_ID;
  }
  if (foundLen != nullptr) {
    *foundLen = 0;
  }
  return DeviceCommunicationIF::NO_REPLY_RECEIVED;
}

ReturnValue_t WebcamDeviceHandler::interpretDeviceReply(DeviceCommandId_t id, const uint8_t *packet) {
  // TODO: Decode incoming telemetry and cast to datapools. Placeholders inserted.
  // (void)id;
  // (void)packet;
  return returnvalue::OK;
}

ReturnValue_t WebcamDeviceHandler::getParameter(uint8_t domainId, uint8_t parameterId,
                                                ParameterWrapper *parameterWrapper,
                                                const ParameterWrapper *newValues,
                                                uint16_t startAtIndex) {
  // Parameter queries allow on-board software to inspect or update handler
  // configuration. Eventually we will expose streaming parameters such as
  // resolution, exposure or compression settings through this interface.
  //
  // TODO: Same here, cast later on.
  // (void)domainId;
  // (void)parameterId;
  // (void)parameterWrapper;
  // (void)newValues;
  // (void)startAtIndex;
  return returnvalue::OK;
}
