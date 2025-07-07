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
// * Message sent from the OpenFlow Switch back to an RSU.
// * It encapsulates an original packet (e.g., a WSM) that the RSU should
// * transmit wirelessly to a vehicle, and includes routing information.
// */
message FromOpenFlowSwitchMessage extends cPacket { // Changed from Packet to cPacket
    cPacket *originalPacket;         // Changed from Packet* to cPacket*
    string rsuDestinationId;        // Identifier of the RSU that should forward this packet (e.g., module full path)
    string vehicleDestinationAddress; // The actual destination address of the vehicle (e.g., its IP or MAC)
}
