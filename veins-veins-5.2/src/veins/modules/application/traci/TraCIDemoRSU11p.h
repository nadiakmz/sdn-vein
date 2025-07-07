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

#pragma once

// Forward declare custom message type for the new method
#ifndef VEINS_TRACIDEMORSUNOP11P_H_
#define VEINS_TRACIDEMORSUNOP11P_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"

#include "veins/modules/messages/BaseFrame1609_4_m.h"        // For BaseFrame1609_4
#include "veins/modules/messages/DemoServiceAdvertisement_m.h" // For DemoServiceAdvertisment
#include "veins/modules/messages/DemoSafetyMessage_m.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

// For your custom OpenFlow messages
#include "openflow/messages/FromOpenFlowSwitchMessage_m.h"
#include "openflow/messages/ToOpenFlowSwitchMessage_m.h"

namespace openflow {
class FromOpenFlowSwitchMessage;
}
namespace veins {

/**
 * @brief Application layer for an RSU to demonstrate TraCI (SUMO) integration
 * and OpenFlow interaction via an INET network stack.
 */
class VEINS_API TraCIDemoRSU11p : public DemoBaseApplLayer {
protected:
    inet::UDPSocket udpSocket;
protected:
    void onWSM(BaseFrame1609_4* wsm) override;
//    void onWSA(DemoServiceAdvertisment* wsa) override;
//    void handleMessageFromOpenFlowSwitch(openflow::FromOpenFlowSwitchMessage* msg);
    void handleMessage(cMessage* msg) override;
//    void handleBSM(DemoSafetyMessage* bsm);
//    void handleHostStateNotification(HostState::States state);
//    void handleSelfMessage(cMessage* msg);

public:
    virtual void initialize(int stage) override;
};

} // namespace veins

#endif /* VEINS_TRACIDEMORSUNOP11P_H_ */
