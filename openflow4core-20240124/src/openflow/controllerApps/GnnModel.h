//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 


#ifndef OPENFLOW_CONTROLLERAPPS_GNNMODEL_H_
#define OPENFLOW_CONTROLLERAPPS_GNNMODEL_H_

#include <omnetpp.h>
#include <unordered_map>
#include "inet/transportlayer/contract/tcp/TCPSocket.h"
#include "inet/common/INETDefs.h"
#include "inet/controller_messages/ControllerCommand_m.h"
#include "openflow/controllerApps/AbstractControllerApp.h"
#include <fstream>
//#include <nlohmann/json.hpp>
//using json = nlohmann::json;



/**
 * TODO - Generated class
 */
namespace openflow{
class GnnModel : public AbstractControllerApp
{
  private:
    cMessage* inferTimer = nullptr;
    simtime_t inferInterval;
    std::string pyExec, inferScript, modelPath, mergedCsv, rsuPositions, outDecisions, mergeScript, inference_outputs,inference_overhead, controller_overhead, clusters_output, files1, files2, files3, files4 ;
    int ctrl_msgs_sent = 0;
    struct RState { int currentCH = -1; double lastSwitch = -1e9; };
    std::unordered_map<std::string, RState> rsuState; // key: rsuIP
//    std::map<std::string, std::pair<int, std::vector<int>>> lastClusterState;
    std::map<std::string, std::vector<std::pair<int, std::vector<int>>>> lastClusterState;

    std::map<std::string, std::vector<int>> lastAttachState;

    std::map<std::string, cModule*> ipToRsu;

  protected:

    virtual void handleMessage(cMessage *msg) override;
    void finish() override;
    void sendReroute(int vehicleID, const std::string& targetIP,const std::string& srcRSU,  std::string& rrTime);
    void sendAttachBatch(const std::vector<int>& vehicles, const std::string& targetRSU);
    void applyDecisions(const std::string& path, double simNow);
    void runInferenceOnce();
    inline void appendCsv(const std::string& path, const std::string& header, const std::string& line);
    void writeControllerOverhead(double t_launch_ms, double t_apply_ms, int hasDecision);
    void applyClusters(const std::string& clustersFile, double simNow);
    void sendClusterInfo(const std::string& rsuIP, int clusterHead, const std::vector<int>& members);
    void sendDetachBatch(const std::vector<int>& vehicles, const std::string& targetRSU, std::string& rrTime);
  public:
    virtual void initialize() override;

};
}

#endif
