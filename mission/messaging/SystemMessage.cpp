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

#include "mission/messaging/SystemMessage.h"

#include "fsfw/ipc/CommandMessage.h"

namespace messagetypes {

    void clearMissionMessage(CommandMessage* message) {
        if (message == nullptr) {
            return;
        }
        message->clear();
    }

}  // namespace messagetypes