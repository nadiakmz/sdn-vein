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

    packetsReceived = 0;
    districtID = par("districtID");

    const char* logFileName = par("vehicleDataFileName").stringValue();;
    vehicleDataFile.open(logFileName);
    EV_ERROR<< "test app logs: "<<logFileName<< endl;
    vehicleDataFile << "simTime,districtID,rsuIP,vehicleID,packetSize,senderPosX,senderPosY,endToEndDelay,senderSpeed,senderDirection\n";
    // This condition is true if the stream is in a failed state
    if (!vehicleDataFile.is_open()) {
        EV_ERROR<< "Error: Could not open file: " << logFileName << std::endl;
    }
    logBuffer.reserve(BATCH_SIZE);
    inferLocalTimer = new cMessage("inferLocalTimer");
    scheduleAt(simTime() + inferLocalInterval, inferLocalTimer);
}

void LoggingApp::finish() {
    recordScalar("packetsReceived", packetsReceived);
    flushBuffer();
    if (vehicleDataFile.is_open()) {
        vehicleDataFile.close();
    }
}
void LoggingApp::handleMessage(cMessage *msg)
    {
        // TODO - Generated method body
        if (msg == inferLocalTimer) {
            flushBuffer();
            scheduleAt(simTime() + inferLocalInterval, inferLocalTimer);
        } else {
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

                  // Calculate End-to-End Delay (Latency)
                  simtime_t creationTime = vehicleMessage->getTimestamp();
                  simtime_t delay = time - creationTime;

                  ss << time << "," << districtID << "," << rsuIP << "," << vehicleID << ","
                          << packetSize << "," << senderPos.x << "," << senderPos.y
                          << "," << delay<< ","<<speed<<","<<direction;

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
