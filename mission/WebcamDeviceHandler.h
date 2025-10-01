# pragma once

#include <fsfw/devicehandlers/DeviceHandlerBase.h>
#include <fsfw/devicehandlers/CookieIF.h>
#include <fsfw/parameters/ParameterWrapper.h>
#include <fsfw/returnvalues/returnvalue.h>
#include <cstdint>
/*
 * Deriviving the DeviceHandlerBase
 */
class WebcamDeviceHandler : public DeviceHandlerBase {
public:
    using DeviceHandlerBase::DeviceHandlerBase;
    void doStartUp() override;
    void doShutDown() override;
protected:
    ReturnValue_t buildTransitionDeviceCommand(DeviceCommandId_t deviceCommand) override;
    ReturnValue_t buildNormalDeviceCommand(DeviceCommandId_t deviceCommand) override;
    void fillCommandAndReplyMap() override;
    ReturnValue_t scanForReply(const uint8_t *data, size_t len, DeviceCommandId_t *foundId, size_t *foundLen) override;
    ReturnValue_t interpretDeviceReply(DeviceCommandId_t id, const uint8_t *packet) override;
    ReturnValue_t getParameter(uint8_t domainId, uint8_t parameterId, ParameterWrapper *parameterWrapper, const ParameterWrapper *newValues, uint16_t startAtIndex) override;
};
