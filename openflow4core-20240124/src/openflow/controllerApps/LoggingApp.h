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

  protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *details) override;
};

}



#endif /* OPENFLOW_CONTROLLERAPPS_LOGGINGAPP_H_ */
