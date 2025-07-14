//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/TraCIDemoRSU11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/obstacle/ObstacleControl.h"

// Standard INET 3.8 headers
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/linklayer/common/MACAddress.h" // FIX 3: Include MACAddress header

#include "veins/modules/messages/BaseFrame1609_4_m.h"

#include "veins/base/modules/BaseMobility.h"
#include "veins/base/utils/FindModule.h"
#include <omnetpp.h>

// Standard library includes
#include <string>
#include <vector>
#include <stdexcept> // For std::runtime_error used in MACAddress constructor

// ** FIX: Placing using namespace statements after all includes **
using namespace veins;
using namespace inet;
using namespace openflow;

Define_Module(TraCIDemoRSU11p);

//const int OPENFLOW_UDP_PORT = 6000;
const int RSU_UDP_PORT = 5000;

void TraCIDemoRSU11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    EV_INFO << "***************************rsu********************++++++++++++++++"<< endl;
    if (stage == 0) {
        EV_INFO << "***************************RSU Initialize********************++++++++++++++++"<< endl;
        udpSocket.setOutputGate(gate("socketOut"));
        udpSocket.bind(L3Address(), RSU_UDP_PORT);
        EV_INFO << "RSU application bound to UDP port " << RSU_UDP_PORT << endl;
    }
}


void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* wsm)
{
    EV_INFO << "***************************onwsm********************++++++++++++++++"<< endl;
    cPacket* payload = wsm->getEncapsulatedPacket()->dup();

    L3Address controllerIp = L3AddressResolver().resolve("open_flow_controller1");
    int controllerPort = par("controllerPort");

    cPacket* udpPacket = new cPacket("DataForController");
    udpPacket->encapsulate(payload);

    EV_INFO << "RSU received WSM, forwarding payload to Controller " << controllerIp << ":" << controllerPort << endl;
    udpSocket.sendTo(udpPacket, controllerIp, controllerPort);

    delete wsm;
}

//void TraCIDemoRSU11p::onWSA(DemoServiceAdvertisment* wsa) {
//    EV_INFO << "RSU " << getParentModule()->getFullName() << " received WSA: " << wsa->getName() << endl;
//    DemoBaseApplLayer::onWSA(wsa);
//}
// This function handles all incoming messages for the module
void TraCIDemoRSU11p::handleMessage(cMessage* msg)
{
    EV_INFO << "------------------------------handlemessage---------"<<endl;


        // Check if it's a wireless message from a vehicle
        if (msg->getArrivalGate() == gate("lowerLayerIn")) {
            if(TraCIDemo11pMessage* v2v_msg = dynamic_cast<TraCIDemo11pMessage*>(msg)){
                EV_INFO << "RSU received TraCIDemo11pMessage from vehicle, forwarding to controller." << endl;

                L3Address controllerIp = L3AddressResolver().resolve("open_flow_controller1");
                int controllerPort = par("controllerPort");
//                udpSocket.sendTo(v2v_msg, controllerIp, controllerPort);

                cPacket* payload = new cPacket("VehicleDataPayload");
                    // Set the size to match the original message for realistic bandwidth simulation
                    payload->setByteLength(v2v_msg->getByteLength());

                    // Send the new, clean packet. The UDPSocket can now attach its own control info.
                    udpSocket.sendTo(payload, controllerIp, controllerPort);

                    // We are now done with the original incoming wireless message.
                    delete v2v_msg;


    //            cPacket* udpPacket = new cPacket("DataForController");
    //            udpPacket->encapsulate(payload);
    //
    //            udpSocket.sendTo(udpPacket, controllerIp, controllerPort);
    //
    //            delete v2v_msg; // Delete the original incoming message
            }
        }
        // Else, check if it's a UDP packet from the switch/controller
        else if (msg->getArrivalGate() == gate("socketIn")) {
            if (cPacket* udpPacket = dynamic_cast<cPacket*>(msg)){
                EV_INFO << "RSU received UDP packet from switch, broadcasting to vehicles." << endl;

                cPacket* appMsgPayload = udpPacket->decapsulate();

                BaseFrame1609_4* frame = new BaseFrame1609_4();
                frame->setChannelNumber(172);
                frame->encapsulate(appMsgPayload);


                sendDown(frame);
                delete udpPacket;
            }
        }else {
            DemoBaseApplLayer::handleMessage(msg);
        }
}
