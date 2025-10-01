//
// Created by noel on 01.10.25.
//
#include "WebcamComIF.h"

#include <fsfw/returnvalues/returnvalue.h>

WebcamComIF::WebcamComIF(object_id_t objectId) : SystemObject(objectId) {}

ReturnValue_t WebcamComIF::initializeInterface(CookieIF *) { return returnvalue::OK; }

ReturnValue_t WebcamComIF::sendMessage(CookieIF *, const uint8_t *, size_t) {
    return returnvalue::OK;
}

ReturnValue_t WebcamComIF::getSendSuccess(CookieIF *) { return returnvalue::OK; }

ReturnValue_t WebcamComIF::requestReceiveMessage(CookieIF *, size_t) {
    return returnvalue::OK;
}

ReturnValue_t WebcamComIF::readReceivedMessage(CookieIF *, uint8_t **buffer,
                                                    size_t *size) {
    if (buffer != nullptr) {
        *buffer = nullptr;
    }
    if (size != nullptr) {
        *size = 0;
    }
    return returnvalue::OK;
}

ReturnValue_t WebcamComIF::getReceiveSuccess(CookieIF *) { return returnvalue::OK; }
#include "WebcamComIF.h"