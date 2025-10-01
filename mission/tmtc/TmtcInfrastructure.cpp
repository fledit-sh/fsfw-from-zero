#include "TmtcInfrastructure.h"

#include <cstdio>
#include <cstring>

#include <fsfw/ipc/MessageQueueIF.h>
#include <fsfw/ipc/MessageQueueMessage.h>
#include <fsfw/ipc/MessageQueueSenderIF.h>
#include <fsfw/ipc/QueueFactory.h>
#include <fsfw/objectmanager/ObjectManager.h>
#include <fsfw/storagemanager/StorageManagerIF.h>
#include <fsfw/timemanager/CdsShortTimeStamper.h>
#include <fsfw/timemanager/TimeReaderIF.h>
#include <fsfw/tmtcpacket/ccsds/PacketId.h>
#include <fsfw/tmtcpacket/ccsds/PacketSeqCtrl.h>
#include <fsfw/tmtcpacket/ccsds/defs.h>
#include <fsfw/tmtcpacket/pus/tc/PusTcCreator.h>
#include <fsfw/tmtcpacket/pus/tm/PusTmReader.h>
#include <fsfw/tmtcservices/PusVerificationReport.h>
#include <fsfw/tmtcservices/TmTcMessage.h>

#include "mission/webcam/WebcamDefinitions.h"

namespace webcam {
namespace {
constexpr size_t MAX_TM_PRINT_BYTES = 32;
}

StubTelemetrySink::StubTelemetrySink(object_id_t objectId) : SystemObject(objectId) {}

StubTelemetrySink::~StubTelemetrySink() {
  if (queue != nullptr) {
    QueueFactory::instance()->deleteMessageQueue(queue);
  }
}

ReturnValue_t StubTelemetrySink::initialize() {
  queue = QueueFactory::instance()->createMessageQueue(QUEUE_DEPTH, MessageQueueMessage::MAX_MESSAGE_SIZE);
  tmStore = ObjectManager::instance()->get<StorageManagerIF>(objects::TM_STORE);
  timeReader = ObjectManager::instance()->get<TimeReaderIF>(objects::TIME_STAMPER);
  return SystemObject::initialize();
}

ReturnValue_t StubTelemetrySink::performOperation(uint8_t) {
  if (queue == nullptr || tmStore == nullptr) {
    return returnvalue::OK;
  }
  TmTcMessage message;
  while (queue->receiveMessage(&message) == returnvalue::OK) {
    store_address_t storeId = message.getStorageId();
    const uint8_t* data = nullptr;
    size_t size = 0;
    if (tmStore->getData(storeId, &data, &size) == returnvalue::OK) {
      PusTmReader reader(timeReader, data, size);
      if (reader.parseDataWithoutCrcCheck() == returnvalue::OK) {
        printf("[TM] service %u subservice %u, %zu bytes\n", reader.getService(), reader.getSubService(),
               reader.getUserDataLen());
      } else {
        const size_t bytesToPrint = size < MAX_TM_PRINT_BYTES ? size : MAX_TM_PRINT_BYTES;
        printf("[TM] Received %zu bytes of telemetry (first %zu bytes): ", size, bytesToPrint);
        for (size_t idx = 0; idx < bytesToPrint; idx++) {
          printf("%02x", data[idx]);
        }
        printf("\n");
      }
      tmStore->deleteData(storeId);
    }
  }
  return returnvalue::OK;
}

MessageQueueId_t StubTelemetrySink::getReportReceptionQueue(uint8_t) {
  if (queue == nullptr) {
    return MessageQueueIF::NO_QUEUE;
  }
  return queue->getId();
}

StubVerificationReceiver::StubVerificationReceiver(object_id_t objectId) : SystemObject(objectId) {}

StubVerificationReceiver::~StubVerificationReceiver() {
  if (queue != nullptr) {
    QueueFactory::instance()->deleteMessageQueue(queue);
  }
}

ReturnValue_t StubVerificationReceiver::initialize() {
  queue = QueueFactory::instance()->createMessageQueue(QUEUE_DEPTH, MessageQueueMessage::MAX_MESSAGE_SIZE);
  return SystemObject::initialize();
}

ReturnValue_t StubVerificationReceiver::performOperation(uint8_t) {
  if (queue == nullptr) {
    return returnvalue::OK;
  }
  PusVerificationMessage message;
  while (queue->receiveMessage(&message) == returnvalue::OK) {
    printf("[TMTC] Verification report %u ack 0x%02x step %u error %d\n", message.getReportId(),
           message.getAckFlags(), message.getStep(), static_cast<int>(message.getErrorCode()));
  }
  return returnvalue::OK;
}

MessageQueueId_t StubVerificationReceiver::getVerificationQueue() {
  if (queue == nullptr) {
    return MessageQueueIF::NO_QUEUE;
  }
  return queue->getId();
}

StubPusDistributor::StubPusDistributor(object_id_t objectId) : SystemObject(objectId) {}

ReturnValue_t StubPusDistributor::initialize() {
  tcStore = ObjectManager::instance()->get<StorageManagerIF>(objects::TC_STORE);
  return SystemObject::initialize();
}

ReturnValue_t StubPusDistributor::registerService(AcceptsTelecommandsIF* service) {
  if (service == nullptr) {
    return returnvalue::FAILED;
  }
  registeredService = service;
  registeredServiceId = service->getIdentifier();
  serviceRequestQueue = service->getRequestQueue();
  return returnvalue::OK;
}

ReturnValue_t StubPusDistributor::sendCommand(uint8_t subservice, const uint8_t* data, size_t dataLen) {
  if (registeredService == nullptr || tcStore == nullptr) {
    return returnvalue::FAILED;
  }
  PacketId packetId(ccsds::PacketType::TC, true, apid);
  PacketSeqCtrl seqCtrl(ccsds::SequenceFlags::UNSEGMENTED, sequenceCounter++);
  SpacePacketParams spParams(packetId, seqCtrl, 0);
  PusTcParams pusParams(registeredServiceId, subservice);
  if (data != nullptr && dataLen > 0) {
    pusParams.setRawAppData(data, dataLen);
  }
  PusTcCreator creator(spParams, pusParams);
  creator.updateSpLengthField();
  const size_t serializedSize = creator.getSerializedSize();
  store_address_t storeId;
  uint8_t* storePtr = nullptr;
  ReturnValue_t result = tcStore->getFreeElement(&storeId, serializedSize, &storePtr);
  if (result != returnvalue::OK) {
    return result;
  }
  uint8_t* writePtr = storePtr;
  size_t size = 0;
  result = creator.serialize(&writePtr, &size, serializedSize, SerializeIF::Endianness::BIG);
  if (result != returnvalue::OK) {
    tcStore->deleteData(storeId);
    return result;
  }
  TmTcMessage message(storeId);
  return MessageQueueSenderIF::sendMessage(serviceRequestQueue, &message);
}

}  // namespace webcam
