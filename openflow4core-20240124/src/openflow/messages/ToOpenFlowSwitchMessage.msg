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
namespace openflow;
///**
// * Message sent from an RSU to the OpenFlow Switch.
// * It encapsulates an original packet (e.g., a WSM) received from a vehicle,
// * and adds metadata for the switch.
// */
message ToOpenFlowSwitchMessage extends cPacket { // Changed from Packet to cPacket
    cPacket *originalPacket;     // Changed from Packet* to cPacket*
    string rsuId;               // Identifier of the RSU sending this packet (e.g., module full path)
    string rsuAddress;          // IP address of the RSU's interface connected to the switch
    string vehicleSourceAddress;    // Original source address of the vehicle that sent the packet (e.g., its IP or MAC)
    string vehicleDestinationAddress; // Original destination address (vehicle IP or MAC)
}
