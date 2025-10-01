//
// Created by noel on 01.10.25.
//
#include "WebcamComIF.h"

#include <fsfw/returnvalues/returnvalue.h>

DummyWebcamComIF::DummyWebcamComIF(object_id_t objectId) : SystemObject(objectId) {}

ReturnValue_t DummyWebcamComIF::initializeInterface(CookieIF *) { return returnvalue::OK; }

ReturnValue_t DummyWebcamComIF::sendMessage(CookieIF *, const uint8_t *, size_t) {
    return returnvalue::OK;
}

ReturnValue_t DummyWebcamComIF::getSendSuccess(CookieIF *) { return returnvalue::OK; }

ReturnValue_t DummyWebcamComIF::requestReceiveMessage(CookieIF *, size_t) {
    return returnvalue::OK;
}

ReturnValue_t DummyWebcamComIF::readReceivedMessage(CookieIF *, uint8_t **buffer,
                                                    size_t *size) {
    if (buffer != nullptr) {
        *buffer = nullptr;
    }
    if (size != nullptr) {
        *size = 0;
    }
    return returnvalue::OK;
}

ReturnValue_t DummyWebcamComIF::getReceiveSuccess(CookieIF *) { return returnvalue::OK; }
#include "WebcamComIF.h"