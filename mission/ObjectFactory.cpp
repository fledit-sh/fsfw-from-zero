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

#include "ObjectFactory.h"

#include <memory>
#include <fsfw/objectmanager/frameworkObjects.h>
#include <fsfw/storagemanager/LocalPool.h>
#include <fsfw/timemanager/CdsShortTimeStamper.h>
#include <fsfw/tmtcservices/VerificationReporter.h>
#include "webcam/WebcamDeviceHandler.h"
#include "mission/tmtc/TmtcInfrastructure.h"
#include "mission/tmtc/WebcamCommandingService.h"
#include "mission/webcam/WebcamComIF.h"
#include "mission/webcam/WebcamCookie.h"
#include "mission/webcam/WebcamDefinitions.h"

namespace {
    std::unique_ptr<LocalPool> ipcStore;
    std::unique_ptr<LocalPool> tcStore;
    std::unique_ptr<LocalPool> tmStore;
    std::unique_ptr<CdsShortTimeStamper> timeStamper;
    std::unique_ptr<VerificationReporter> verificationReporter;
    std::unique_ptr<webcam::StubTelemetrySink> telemetrySink;
    std::unique_ptr<webcam::StubVerificationReceiver> verificationReceiver;
    std::unique_ptr<webcam::StubPusDistributor> pusDistributor;
    std::unique_ptr<WebcamComIF> webcamComIF;
    std::unique_ptr<WebcamCookie> webcamCookie;
    std::unique_ptr<WebcamDeviceHandler> webcamHandler;
    std::unique_ptr<webcam::WebcamCommandingService> webcamService;
    bool serviceRegistered = false;
}

void ObjectFactory::createMissionObjects() {
    if (ipcStore == nullptr) {
        LocalPool::LocalPoolConfig ipcCfg = {{40, 32}, {20, 64}, {10, 128}};
        ipcStore = std::make_unique<LocalPool>(objects::IPC_STORE, ipcCfg, true, true);
    }
    if (tcStore == nullptr) {
        LocalPool::LocalPoolConfig tcCfg = {{20, 256}, {10, 512}};
        tcStore = std::make_unique<LocalPool>(objects::TC_STORE, tcCfg, true, true);
    }
    if (tmStore == nullptr) {
        LocalPool::LocalPoolConfig tmCfg = {{20, 256}, {10, 512}};
        tmStore = std::make_unique<LocalPool>(objects::TM_STORE, tmCfg, true, true);
    }
    if (timeStamper == nullptr) {
        timeStamper = std::make_unique<CdsShortTimeStamper>(objects::TIME_STAMPER);
    }
    if (verificationReceiver == nullptr) {
        verificationReceiver = std::make_unique<webcam::StubVerificationReceiver>(
            webcam::objectIdWebcamVerificationSink);
    }
    if (verificationReporter == nullptr) {
        verificationReporter = std::make_unique<VerificationReporter>(objects::VERIFICATION_REPORTER);
        if (verificationReceiver != nullptr) {
            verificationReporter->setReceiver(*verificationReceiver);
        }
    }
    if (telemetrySink == nullptr) {
        telemetrySink =
            std::make_unique<webcam::StubTelemetrySink>(webcam::objectIdWebcamTelemetrySink);
    }
    if (pusDistributor == nullptr) {
        pusDistributor =
            std::make_unique<webcam::StubPusDistributor>(webcam::objectIdWebcamTcDistributor);
    }
    if (webcamComIF == nullptr) {
        webcamComIF = std::make_unique<WebcamComIF>(
            webcam::objectIdWebcamComIF);
    }
    if (webcamCookie == nullptr && webcamHandler == nullptr) {
        webcamCookie = std::make_unique<WebcamCookie>("/dev/video0", 30.0);
    }
    if (webcamHandler == nullptr && webcamCookie != nullptr) {
        webcamHandler = std::make_unique<WebcamDeviceHandler>(
        webcam::objectIdWebcamHandler, webcam::objectIdWebcamComIF,
        webcamCookie.release(), nullptr, 20);
    }
    if (webcamService == nullptr) {
        if (verificationReporter != nullptr) {
            webcamService = std::make_unique<webcam::WebcamCommandingService>(
                webcam::objectIdWebcamCommandingService, verificationReporter.get());
        } else {
            webcamService = std::make_unique<webcam::WebcamCommandingService>(
                webcam::objectIdWebcamCommandingService);
        }
        webcamService->setPacketSource(webcam::objectIdWebcamTcDistributor);
        webcamService->setPacketDestination(webcam::objectIdWebcamTelemetrySink);
    }
    if (pusDistributor != nullptr && webcamService != nullptr && !serviceRegistered) {
        pusDistributor->registerService(webcamService.get());
        serviceRegistered = true;
    }
}