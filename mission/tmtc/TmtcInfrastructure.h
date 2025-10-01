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

#include <fsfw/objectmanager/SystemObject.h>
#include <fsfw/tasks/ExecutableObjectIF.h>
#include <fsfw/tcdistribution/PUSDistributorIF.h>
#include <fsfw/tmtcservices/AcceptsTelemetryIF.h>
#include <fsfw/tmtcservices/AcceptsVerifyMessageIF.h>

#include <cstddef>

class MessageQueueIF;
class StorageManagerIF;
class TimeReaderIF;
class CdsShortTimeStamper;

namespace webcam {

class StubTelemetrySink : public SystemObject, public AcceptsTelemetryIF, public ExecutableObjectIF {
 public:
  explicit StubTelemetrySink(object_id_t objectId);
  ~StubTelemetrySink() override;

  ReturnValue_t initialize() override;
  ReturnValue_t performOperation(uint8_t operationCode) override;
  MessageQueueId_t getReportReceptionQueue(uint8_t virtualChannel) override;

 private:
  static constexpr size_t QUEUE_DEPTH = 10;
  MessageQueueIF* queue = nullptr;
  StorageManagerIF* tmStore = nullptr;
  TimeReaderIF* timeReader = nullptr;
};

class StubVerificationReceiver : public SystemObject,
                                 public AcceptsVerifyMessageIF,
                                 public ExecutableObjectIF {
 public:
  explicit StubVerificationReceiver(object_id_t objectId);
  ~StubVerificationReceiver() override;

  ReturnValue_t initialize() override;
  ReturnValue_t performOperation(uint8_t operationCode) override;
  MessageQueueId_t getVerificationQueue() override;

 private:
  static constexpr size_t QUEUE_DEPTH = 10;
  MessageQueueIF* queue = nullptr;
};

class StubPusDistributor : public SystemObject, public PUSDistributorIF {
 public:
  explicit StubPusDistributor(object_id_t objectId);

  ReturnValue_t initialize() override;
  ReturnValue_t registerService(AcceptsTelecommandsIF* service) override;

  ReturnValue_t sendCommand(uint8_t subservice, const uint8_t* data = nullptr, size_t dataLen = 0);

 private:
  AcceptsTelecommandsIF* registeredService = nullptr;
  MessageQueueId_t serviceRequestQueue = MessageQueueIF::NO_QUEUE;
  uint8_t registeredServiceId = 0;
  StorageManagerIF* tcStore = nullptr;
  uint16_t sequenceCounter = 0;
  uint16_t apid = 0x01;
};

}  // namespace webcam