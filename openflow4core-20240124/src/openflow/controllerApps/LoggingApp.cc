#include "LoggingApp.h"


// --- ADD these necessary includes for packet decapsulation ---
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "inet/networklayer/ipv4/IPv4Datagram.h"
#include "inet/transportlayer/udp/UDPPacket.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "inet/networklayer/arp/ipv4/ARPPacket_m.h"
#include "openflow/messages/OFP_Packet_Out_m.h"
#include "openflow/messages/OFP_Packet_In_m.h" //Needed for casting
#include <sstream>
// ---

using namespace veins;

namespace openflow {

Define_Module(LoggingApp);

void LoggingApp::initialize() {
    AbstractControllerApp::initialize();
    inferLocalInterval = par("inferLocalInterval");

    metricsPath = par("metricsPath").stdstringValue();
    winStart = simTime();

    networkMetricInterval = par("networkMetricInterval");

    packetsReceived = 0;
    districtID = par("districtID");

    const char* logFileName = par("vehicleDataFileName").stringValue();;
    vehicleDataFile.open(logFileName);
    EV_ERROR<< "test app logs: "<<logFileName<< endl;
    vehicleDataFile << "simTime,districtID,rsuIP,vehicleID,packetSize,senderPosX,senderPosY,endToEndDelay,senderSpeed,senderDirection,hasCluster,sendFromCH,txPktsCumulative\n";
    // This condition is true if the stream is in a failed state
    if (!vehicleDataFile.is_open()) {
        EV_ERROR<< "Error: Could not open file: " << logFileName << std::endl;
    }
    logBuffer.reserve(BATCH_SIZE);
    inferLocalTimer = new cMessage("inferLocalTimer");
    scheduleAt(simTime() + inferLocalInterval, inferLocalTimer);

    networkMetricTimer = new cMessage("networkMetricTimer");
    scheduleAt(simTime() + networkMetricInterval, networkMetricTimer);



//    //    --------svm-------------------
//    svm_rsu_weights = par("svm_rsu_weights").stdstringValue();
//    loadSVMWeights(svm_rsu_weights);
}

void LoggingApp::loadSVMWeights(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        EV_ERROR << "Could not open SVM weights file: " << filename << "\n";
        return;
    }

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        SVMWeights w;
        std::string rsuIP;

        std::getline(ss, rsuIP, ',');
        std::getline(ss, token, ','); w.bias = std::stod(token);
        std::getline(ss, token, ','); w.w_density = std::stod(token);
        std::getline(ss, token, ','); w.w_avgDelay = std::stod(token);
        std::getline(ss, token, ','); w.w_throughput_bps = std::stod(token);
        std::getline(ss, token, ','); w.w_tx_pps = std::stod(token);
        std::getline(ss, token, ','); w.w_rx_pps = std::stod(token);
        std::getline(ss, token, ','); w.w_plr = std::stod(token);
        std::getline(ss, token, ','); w.w_dropCount = std::stod(token);

        rsuWeights[rsuIP] = w;
    }
    file.close();
    EV_INFO << "Loaded " << rsuWeights.size() << " RSU SVM models\n";
}

double LoggingApp::calculateRSUScore(const std::string& rsuIP, const RSUMetrics& m) {
    auto it = rsuWeights.find(rsuIP);
    if (it == rsuWeights.end()) {
        EV_WARN << "No SVM weights for RSU " << rsuIP << "\n";
        return -std::numeric_limits<double>::infinity();
    }

    const SVMWeights& w = it->second;

    double score = w.bias
            + w.w_density        * m.density
            + w.w_avgDelay       * m.avgDelay
            + w.w_throughput_bps * m.throughput_bps
            + w.w_tx_pps         * m.tx_pps
            + w.w_rx_pps         * m.rx_pps
            + w.w_plr            * m.plr
            + w.w_dropCount      * m.dropCount;

    return score;
}
//void LoggingApp::updateVehicleEntry(const std::string& vid,
//        int txPktsCumulative,
//        const std::string& rsuIP,
//        const RSUMetrics& metrics)
//{
//    double score = calculateRSUScore(rsuIP, metrics);
//
//    VehicleKey key{vid, txPktsCumulative};
//
//    auto it = vehicleTable.find(key);
//    if (it == vehicleTable.end()) {
//        // First time we see this (vid, packet)
//        vehicleTable[key] = {rsuIP, score};
//        rxPktsWin++;
//        rxBytesWin += packetSize;
//        vehTxCumLatest[vehId] = txPktsCumulative;
//    } else {
//        // Already exists: check if new RSU has better score
//        if (score > it->second.score) {
//            it->second.rsu   = rsuIP;
//            it->second.score = score;
//        }
//    }
//}

void LoggingApp::onDeliveredPacket(long packetSize /*, int vehIdIfKnown*/) {
    rxPktsWin++;
    rxBytesWin += packetSize;
}

void LoggingApp::onVehBeaconTx(long vehId, uint64_t txPktsCumulative, long packetSize) {
    if (vehTxCumLatest[vehId] != txPktsCumulative){
        vehTxCumLatest[vehId] = txPktsCumulative;
        rxPktsWin++;
        rxBytesWin += packetSize;
    }

    //    lastBeaconAt[vehId]   = simTime();
    // You likely already parse beacons somewhere; just call this from there.
}

void LoggingApp::finish() {
    recordScalar("packetsReceived", packetsReceived);
    flushBuffer();
    if (vehicleDataFile.is_open()) {
        vehicleDataFile.close();
    }
    writeNetworkMetric();
    //    if (metricsPath.is_open()) {
    //        metricsPath.close();
    //        }
}
void LoggingApp::writeNetworkMetric(){

    //    double dt = inferLocalInterval.dbl();
    simtime_t now = simTime();
    double dt = (now - winStart).dbl();

    // --- compute TX pkts in this window from cumulative counters ---
    uint64_t txPktsWinSum = 0;

    //    std::ostringstream debug;
    std::ofstream f(metricsPath, std::ios::app);

    for (auto& kv : vehTxCumLatest) {
        int vid = kv.first;
        uint64_t latest = kv.second;
        uint64_t base   = vehTxCumBaseline[vid];  // default 0 on first window
        // naive wrap guard: if latest < base (wrap/reset), treat as latest
        uint64_t delta  = (latest >= base) ? (latest - base) : latest;
        txPktsWinSum += delta;
    }

    //    uint64_t txPktsInWindow = txPktsWinSum - lastTotalTxPkts;
    uint64_t rxPktsInWindow = rxPktsWin - lastTotalRxPkts;
    // --- metrics ---
    double tx_pps        = txPktsWinSum / dt;
    double rx_pps        = rxPktsInWindow     / dt;
    double throughput_bps= (8.0 * rxBytesWin) / dt;
    double plr           = (txPktsWinSum > 0) ? (1.0 - (double)rxPktsInWindow / (double)txPktsWinSum) : 0.0;

    std::ostringstream ln;
    ln.setf(std::ios::fixed); ln.precision(6);
    ln << (int)now.dbl() << ','
            << txPktsWinSum << ','
            << rxPktsInWindow << ','
            << tx_pps << ','
            << rx_pps << ','
            << throughput_bps << ','
            << plr;

    struct stat st;
    bool writeHeader = (stat(metricsPath.c_str(), &st) != 0 || st.st_size == 0);

    //    std::ofstream f(metricsPath, std::ios::app);
    if (writeHeader) f << "time,tx_pkts,rx_pkts,tx_pps,rx_pps,throughput_bps,plr" << "\n";
    //        if (writeHeader) f << "simTime,myIP,DetachTable size,active,expired" << "\n";
    f << ln.str() << "\n";

    winStart = now;
    vehTxCumBaseline = vehTxCumLatest; // new baseline

    lastTotalRxPkts = rxPktsWin;
    //    rxPktsWin = rxBytesWin = 0;
    rxBytesWin = 0;
    //    lastTotalTxPkts = txPktsWinSum;



//    --------------svm-----------
//    rsuLastAvgDelay.clear();
//
//    for (auto& kv : rsuDelayMap) {
//        std::string ip = kv.first;
//        DelayStats tmp = kv.second;
//        simtime_t totalDelay = tmp.totalDelay;
//        int count = tmp.count;
//
//        rsuLastAvgDelay[ip] = (count > 0) ? (totalDelay.dbl() / count) : 0.0;
//
//    }

}
void LoggingApp::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    if (msg == inferLocalTimer) {
        flushBuffer();

        scheduleAt(simTime() + inferLocalInterval, inferLocalTimer);
    } else if (msg == networkMetricTimer){
        writeNetworkMetric();
        scheduleAt(simTime() + networkMetricInterval, networkMetricTimer);
    }else {
        delete msg;
    }
}
void LoggingApp::receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *details) {
    EV << "OFA_controller::LoggingApp:recieved++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    if (id == PacketInSignalId) {

        // Cast the incoming message to the correct Packet_In type
        OFP_Packet_In* packetIn = check_and_cast<OFP_Packet_In*>(static_cast<cObject*>(obj));

        // The Packet_In message encapsulates the entire Ethernet frame from the RSU
        cPacket* encapsulatedPacket = packetIn->getEncapsulatedPacket();

        EV_ERROR<< "encapsulated packet "<<encapsulatedPacket <<endl;
        if (!encapsulatedPacket) return;
        EthernetIIFrame* ethFrame = dynamic_cast<EthernetIIFrame*>(encapsulatedPacket);
        if (!ethFrame) {
            EV_WARN << "Controller received a non-Ethernet packet. Discarding." << endl;
            return;
        }

        if (ethFrame->getEtherType() == 0x0800) {
            EV_INFO << "Controller received an Ethernet data packet. Logging vehicle data.\n";

            IPv4Datagram* ipDatagram = dynamic_cast<IPv4Datagram*>(ethFrame->getEncapsulatedPacket());
            EV_ERROR<< "ipdatagram: " <<ipDatagram <<endl;
            if (!ipDatagram) return;

            UDPPacket* udpPacket = dynamic_cast<UDPPacket*>(ipDatagram->getEncapsulatedPacket());
            EV_ERROR<< "udp: " << udpPacket <<endl;
            if (!udpPacket) return;

            // The final payload inside the UDP packet is our TraCIDemo11pMessage
            TraCIDemo11pMessage* vehicleMessage = dynamic_cast<TraCIDemo11pMessage*>(udpPacket->getEncapsulatedPacket());
            EV_ERROR<< "vehiclemessage: " << vehicleMessage<<endl;
            if (!vehicleMessage) return;

            packetsReceived++;

            std::stringstream ss;

            // Now we can extract all the information
            simtime_t time = simTime();
            L3Address rsuIP = ipDatagram->getSrcAddress(); // Get the source IP from the IP header
            long vehicleID = vehicleMessage->getSenderAddress();
            long packetSize = vehicleMessage->getByteLength();
            std::string data = vehicleMessage->getDemoData();
            int sequenceNum = vehicleMessage->getSerial();
            Coord senderPos = vehicleMessage->getNodeMobilityCoord();
            double speed = vehicleMessage->getSpeed();
            double direction = vehicleMessage->getDirection();
            bool hasCluster =  vehicleMessage->getHasCluster();
            bool sendFromCH = vehicleMessage->getSendFromCH();
            uint64_t txPktsCumulative = vehicleMessage->getTxPktsCumulative();
            onVehBeaconTx(vehicleID, txPktsCumulative, packetSize); //my approach--------------------------------------------------
            // Calculate End-to-End Delay (Latency)
            simtime_t creationTime = vehicleMessage->getTimestamp();
            simtime_t delay = time - creationTime;

            //            -------------------svm------------------
//            auto& d = rsuDelayMap[rsuIP.str()];
//            d.totalDelay += delay;
//            d.count += 1;
//
//            double lastAvgDelay = 0;
//            auto it1 = rsuLastAvgDelay.find(rsuIP.str());
//            if (it1 != rsuLastAvgDelay.end()) {
//                lastAvgDelay = it1->second;
//            }
//
//            RSUMetrics metrics;
//            metrics.density = vehicleMessage->getSVM_density();
//            metrics.dropCount = vehicleMessage->getSVM_dropCount();
//            metrics.plr = vehicleMessage->getSVM_plr();
//            metrics.rx_pps = vehicleMessage->getSVM_rx_pps();
//            metrics.tx_pps = vehicleMessage->getSVM_tx_pps();
//            metrics.throughput_bps = vehicleMessage->getSVM_throughput_bps();
//            metrics.avgDelay = lastAvgDelay;
//
//            double score = calculateRSUScore(rsuIP.str(), metrics);
//
//            VehicleKey key{vehicleID, txPktsCumulative};
//
//            auto it = vehicleTable.find(key);
//            if (it == vehicleTable.end()) {
//                // First time we see this (vid, packet)
//                vehicleTable[key] = {rsuIP.str(), score};
//                rxPktsWin++;
//                rxBytesWin += packetSize;
//                vehTxCumLatest[vehicleID] = txPktsCumulative;
//            } else {
//                // Already exists: check if new RSU has better score
//                if (score > it->second.score) {
//                    it->second.rsu   = rsuIP.str();
//                    it->second.score = score;
//                }else{
//                    return;
//                }
//            }
            //            --------------------svm-------------------



            ss << time << "," << districtID << "," << rsuIP << "," << vehicleID << ","
                    << packetSize << "," << senderPos.x << "," << senderPos.y
                    << "," << delay<< ","<<speed<<","<<direction
                    << "," << hasCluster << "," << sendFromCH
                    << "," << txPktsCumulative;
//                    << "," << score; //-------------------------svm----------------

            logBuffer.push_back(ss.str());
            if (logBuffer.size() >= BATCH_SIZE) {
                flushBuffer();
            }
            //                  vehicleDataFile << time << "," << rsuIP << "," << vehicleID << "," << packetSize << "," << senderPos.x << "," << senderPos.y << "," << delay<< ","<<speed<<","<<direction <<std::endl;
            //                  vehicleDataFile.flush(); // Ensure data is written immediately for debugging

            EV_INFO << "Received data: vehicleID=" << vehicleID
                    << ", sequenceNum=" << sequenceNum
                    << ", data='" << data <<
                    "' packetsReceived=" << packetsReceived << endl;
            //                  EV_ERROR<< "log: " << time << "," << rsuIP << "," << vehicleID << "," << packetSize <<"," << delay << ","<<speed<<","<<direction<< std::endl;
        }
    }

    // --- START OF LOGGING LOGIC ---


    // --- END OF LOGGING LOGIC ---
}
void LoggingApp::flushBuffer() {
    EV_INFO << "Writing a batch of " << logBuffer.size() << " log entries to file." << endl;
    for (const auto& line : logBuffer) {
        vehicleDataFile << line << std::endl;
    }
    // Clear the buffer for the next batch
    logBuffer.clear();
}
} // namespace openflow
