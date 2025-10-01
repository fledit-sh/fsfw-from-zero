#include "mission/SystemMessage.h"

#include "fsfw/ipc/CommandMessage.h"

namespace messagetypes {

    void clearMissionMessage(CommandMessage* message) {
        if (message == nullptr) {
            return;
        }
        message->clear();
    }

}  // namespace messagetypes