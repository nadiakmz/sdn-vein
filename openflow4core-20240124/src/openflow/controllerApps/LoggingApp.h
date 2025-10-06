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
#include <unordered_map>   // <-- required for std::unordered_map
#include <string>
#include <utility>  // for std::pair
#include <limits>

namespace openflow {

class LoggingApp : public AbstractControllerApp
{
    struct SVMWeights {
        double bias;
        double w_density;
        double w_avgDelay;
        double w_throughput_bps;
        double w_tx_pps;
        double w_rx_pps;
        double w_plr;
        double w_dropCount;
    };
    struct RSUMetrics {
        double density;
        double avgDelay;
        double throughput_bps;
        double tx_pps;
        double rx_pps;
        double plr;
        double dropCount;
    };

    struct VehicleKey {
        long vid;
        uint64_t txPktsCumulative;

        bool operator==(const VehicleKey& other) const {
            return vid == other.vid && txPktsCumulative == other.txPktsCumulative;
        }
    };

    // Hash function for VehicleKey
    struct VehicleKeyHash {
        std::size_t operator()(const VehicleKey& k) const {
            return std::hash<long>()(k.vid) ^ (std::hash<uint64_t>()(k.txPktsCumulative) << 1);
        }
    };

    struct VehicleEntry {
        std::string rsu;
        double score;
    };
    struct DelayStats {
        simtime_t totalDelay;
        int count;

    };

    std::unordered_map<std::string, DelayStats> rsuDelayMap;
    std::unordered_map<std::string, double> rsuLastAvgDelay;
//------------------------------------svm----------------------

  private:
    std::ofstream vehicleDataFile;
    long packetsReceived;
    std::vector<std::string> logBuffer;
    const int BATCH_SIZE = 1;
    int districtID;
    cMessage* inferLocalTimer = nullptr;
    simtime_t inferLocalInterval;
    simtime_t networkMetricInterval;
    cMessage* networkMetricTimer = nullptr;

    // ---- window accumulators (RX observed at this controller) ----
    uint64_t rxPktsWin = 0;
    uint64_t rxBytesWin = 0;

    uint64_t totalTxPkts = 0;
    uint64_t totalRxPkts = 0;
    uint64_t lastTotalTxPkts = 0;
    uint64_t lastTotalRxPkts = 0;


        // ---- vehicle TX cumulative counters from beacons ----
    std::unordered_map<int, uint64_t> vehTxCumLatest;   // vehId -> cumulative TX pkts
    std::unordered_map<int, uint64_t> vehTxCumBaseline; // snapshot at last window edge

        // ---- timer (10s model window) ----
    cMessage* modelTick = nullptr;
    static constexpr double MODEL_WIN_S = 10.0;  // align with your model interval
    static constexpr double BEACON_S    = 3.0;   // FYI; not used in math here
    std::string metricsPath;
    simtime_t winStart;

//-------------------------svm-------------------
    std::unordered_map<std::string, SVMWeights> rsuWeights;  //svm
    std::string svm_rsu_weights;
    std::unordered_map<VehicleKey, VehicleEntry, VehicleKeyHash> vehicleTable;
    //-------------------------svm-------------------

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *details) override;
    void flushBuffer();
    void onVehBeaconTx(long vehId, uint64_t txPktsCumulative, long packetSize);
    void onDeliveredPacket(long packetSize /*, int vehIdIfKnown*/);
    void writeNetworkMetric();

    void loadSVMWeights(const std::string& filename);
    double calculateRSUScore(const std::string& rsuIP, const RSUMetrics& m);
//    void updateVehicleEntry(const std::string& vid,
//            int txPktsCumulative,
//            const std::string& rsuIP,
//            const RSUMetrics& metrics);
};

}



#endif /* OPENFLOW_CONTROLLERAPPS_LOGGINGAPP_H_ */
