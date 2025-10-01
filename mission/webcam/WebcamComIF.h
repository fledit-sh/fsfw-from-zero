#pragma once

#include <cstddef>

#include <fsfw/devicehandlers/DeviceCommunicationIF.h>
#include <fsfw/objectmanager/SystemObject.h>

class DummyWebcamComIF : public DeviceCommunicationIF, public SystemObject {
public:
    explicit DummyWebcamComIF(object_id_t objectId);

    ReturnValue_t initializeInterface(CookieIF *cookie) override;
    ReturnValue_t sendMessage(CookieIF *cookie, const uint8_t *sendData,
                              size_t sendLen) override;
    ReturnValue_t getSendSuccess(CookieIF *cookie) override;
    ReturnValue_t requestReceiveMessage(CookieIF *cookie, size_t requestLen) override;
    ReturnValue_t readReceivedMessage(CookieIF *cookie, uint8_t **buffer,
                                      size_t *size) override;
    ReturnValue_t getReceiveSuccess(CookieIF *cookie) override;
};