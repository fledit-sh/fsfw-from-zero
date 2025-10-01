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

#pragma once

#include <cstddef>

#include <fsfw/devicehandlers/DeviceCommunicationIF.h>
#include <fsfw/objectmanager/SystemObject.h>

class WebcamComIF : public DeviceCommunicationIF, public SystemObject {
public:
    explicit WebcamComIF(object_id_t objectId);

    ReturnValue_t initializeInterface(CookieIF *cookie) override;
    ReturnValue_t sendMessage(CookieIF *cookie, const uint8_t *sendData,
                              size_t sendLen) override;
    ReturnValue_t getSendSuccess(CookieIF *cookie) override;
    ReturnValue_t requestReceiveMessage(CookieIF *cookie, size_t requestLen) override;
    ReturnValue_t readReceivedMessage(CookieIF *cookie, uint8_t **buffer,
                                      size_t *size) override;
};