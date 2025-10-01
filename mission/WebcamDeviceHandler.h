# pragma once

#include <fsfw/devicehandlers/DeviceHandlerBase.h>
#include <fsfw/devicehandlers/CookieIF.h>
#include <fsfw/parameters/ParameterWrapper.h>
#include <fsfw/returnvalues/returnvalue.h>
#include <cstdint>
#include <cstddef>
/*
 * Deriviving the DeviceHandlerBase
 */
class WebcamDeviceHandler : public DeviceHandlerBase {
public:
    WebcamDeviceHandler(object_id_t objectId, object_id_t deviceCommunication = 0,
                    CookieIF *comCookie = nullptr, FailureIsolationBase *fdirInstance = nullptr,
                    size_t cmdQueueSize = 20);
    double currentFrameRate = 0.0;   // latest reported framerate
    double requestedFrameRate = 0.0; // framerate to set ie from tmtc
    bool snapshotRequested = false;  //
    void doStartUp() override; //TODO: implement HW startup logic like getting the webcamhanlder
    void doShutDown() override; //TODO: implement HW shutdown logic like releasing the webcamhandler
protected:
    ReturnValue_t buildTransitionDeviceCommand(DeviceCommandId_t deviceCommand) override; //
    ReturnValue_t buildNormalDeviceCommand(DeviceCommandId_t deviceCommand) override; //
    void fillCommandAndReplyMap() override;  //
    ReturnValue_t scanForReply(const uint8_t *data, size_t len, DeviceCommandId_t *foundId, size_t *foundLen) override; //
    ReturnValue_t interpretDeviceReply(DeviceCommandId_t id, const uint8_t *packet) override; //
    ReturnValue_t getParameter(uint8_t domainId, uint8_t parameterId, ParameterWrapper *parameterWrapper, const ParameterWrapper *newValues, uint16_t startAtIndex) override;
};
