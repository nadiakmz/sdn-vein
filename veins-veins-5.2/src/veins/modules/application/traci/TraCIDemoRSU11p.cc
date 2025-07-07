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

    if (stage == INITSTAGE_APPLICATION_LAYER) {
        udpSocket.setOutputGate(gate("lowerLayerOut"));
        udpSocket.bind(L3Address(), RSU_UDP_PORT);
        EV_INFO << "RSU application bound to UDP port " << RSU_UDP_PORT << endl;
    }
}


void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* wsm)
{
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
    if (msg->getArrivalGate() == gate("lowerLayerIn")) {
        EV_INFO << "RSU received UDP packet from switch, preparing to broadcast wirelessly." << endl;

        cPacket* udpPacket = dynamic_cast<cPacket*>(msg);
        if (!udpPacket) {
            EV_ERROR << "Message from UDP is not a cPacket. Dropping." << endl;
            delete msg;
            return;
        }

        // FIX 1: Use the decapsulated packet directly as the application payload
         cPacket* appMsgPayload = udpPacket->decapsulate();

         BaseFrame1609_4* frame = new BaseFrame1609_4();
         frame->setChannelNumber(172);
               // FIX 3: Use MACAddress instead of L2Address
//         frame->setRecipientAddress(MACAddress::BROADCAST_ADDRESS);
         // FIX 2: Removed the setPriority() call

         frame->encapsulate(appMsgPayload); // Encapsulate the actual payload

         sendDown(frame);

         delete udpPacket;
         return;
    }

    DemoBaseApplLayer::handleMessage(msg);
}
