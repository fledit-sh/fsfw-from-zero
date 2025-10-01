#pragma once

#include "fsfw/datapoollocal/localPoolDefinitions.h"
#include "fsfw/devicehandlers/DeviceHandlerBase.h"
#include "fsfw/parameters/HasParametersIF.h"
#include "fsfw/parameters/ParameterWrapper.h"
#include "fsfw/returnvalues/returnvalue.h"

#include <string_view>

/**
 * @brief Minimal FSFW device handler
 */
class WebcamDeviceHandler : public DeviceHandlerBase {
 public:
  static constexpr object_id_t DEFAULT_OBJECT_ID = 0xC0FFEE01;
  explicit WebcamDeviceHandler(object_id_t objectId = DEFAULT_OBJECT_ID);

 protected:
  /** Command IDs*/
  enum CommandIds : DeviceCommandId_t {
    CAPTURE_FRAME = 0x01,
    SET_EXPOSURE = 0x02,
    DUMP_STATUS = 0x03,
  };

  /** Parameter IDs*/
  enum ParameterIds : ParameterId_t {
    PARAM_EXPOSURE_TIME = 0x1000,
    PARAM_FRAME_INTERVAL = 0x1001,
  };

  // DeviceHandlerBase hooks
  void doStartUp() override;
  void doShutDown() override;
  ReturnValue_t buildNormalDeviceCommand(DeviceCommandId_t deviceCommand);
  ReturnValue_t buildTransitionDeviceCommand(DeviceCommandId_t deviceCommand);
  ReturnValue_t buildCommandFromCommand(DeviceCommandId_t deviceCommand, const uint8_t* commandData, size_t commandDataLen) override;
  ReturnValue_t scanForReply(const uint8_t* start, size_t len);
  ReturnValue_t interpretDeviceReply(DeviceCommandId_t deviceCommand, const uint8_t* reply, size_t replyLen);
  void fillCommandAndReplyMap() override;
  void modeChanged() override;
  ReturnValue_t initializeLocalDataPool(localpool::DataPool& localDataPoolMap);
  ReturnValue_t getParameter(uint8_t domainId, uint8_t uniqueIdentifier, ParameterWrapper* parameterWrapper,
                             const ParameterWrapper* newValues, uint16_t startAtIndex) override;

 private:
  uint32_t exposureTimeMs = 33;      //!< placeholder for specification commandment 7
  uint32_t frameIntervalMs = 1000;   //!< placeholder for specification commandment 7
};