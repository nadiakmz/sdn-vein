/*
 * LoggingApp.h
 *
 *  Created on: Jul 19, 2025
 *      Author: nadia
 */

#ifndef OPENFLOW_CONTROLLERAPPS_LOGGINGAPP_H_
#define OPENFLOW_CONTROLLERAPPS_LOGGINGAPP_H_

#include "openflow/controllerApps/AbstractControllerApp.h"
#include <fstream>

namespace openflow {

class LoggingApp : public AbstractControllerApp
{
  private:
    std::ofstream vehicleDataFile;
    long packetsReceived;
    std::vector<std::string> logBuffer;
    const int BATCH_SIZE = 1;
    int districtID;
    cMessage* inferLocalTimer = nullptr;
    simtime_t inferLocalInterval;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *details) override;
    void flushBuffer();
};

}



#endif /* OPENFLOW_CONTROLLERAPPS_LOGGINGAPP_H_ */
