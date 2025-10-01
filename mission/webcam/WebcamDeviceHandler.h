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

# pragma once

#include <fsfw/devicehandlers/DeviceHandlerBase.h>
#include <fsfw/devicehandlers/CookieIF.h>
#include <fsfw/parameters/ParameterWrapper.h>
#include <fsfw/returnvalues/returnvalue.h>
#include <array>
#include <cstddef>
#include <cstdint>

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
    ReturnValue_t buildTransitionDeviceCommand(DeviceCommandId_t *deviceCommand) override;
    ReturnValue_t buildNormalDeviceCommand(DeviceCommandId_t *deviceCommand) override;
    void fillCommandAndReplyMap() override;
    uint32_t getTransitionDelayMs(Mode_t modeFrom, Mode_t modeTo) override;
    ReturnValue_t scanForReply(const uint8_t *data, size_t len, DeviceCommandId_t *foundId, size_t *foundLen) override;
    ReturnValue_t interpretDeviceReply(DeviceCommandId_t id, const uint8_t *packet) override;
    ReturnValue_t getParameter(uint8_t domainId, uint8_t parameterId, ParameterWrapper *parameterWrapper, const ParameterWrapper *newValues, uint16_t startAtIndex) override;
    ReturnValue_t buildCommandFromCommand(DeviceCommandId_t deviceCommand, const uint8_t *commandData, size_t commandDataLen) override;
    ReturnValue_t letChildHandleMessage(CommandMessage *message) override;
private:
    void prepareReply(DeviceCommandId_t commandId);
    bool devicePowered = false;
    bool transitionCommandPending = false;
    bool frameRateCommandPending = false;
    bool frameRateQueryPending = false;
    bool snapshotInProgress = false;
    bool replyReady = false;
    DeviceCommandId_t fabricatedReplyId = DeviceHandlerIF::NO_COMMAND_ID;
    std::array<uint8_t, 1> dummyCommandBuffer{};
};
