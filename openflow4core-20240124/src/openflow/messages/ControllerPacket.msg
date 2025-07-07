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

//
// TODO generated message class
//
packet ControllerPacket {
    cPacket *originalVehicleMessage; // Encapsulates the TraCIDemo11pMessage or BaseFrame1609_4
    string rsuId;                    // Full path of the RSU that sent the packet
    string sourceVehicleId;          // ID/Address of the original sending vehicle
    string destinationVehicleId;     // ID/Address of the target vehicle
    string controllerInstruction;    // e.g., "FORWARD_TO_RSU", "DROP", "LOCAL_DELIVER"
    string targetRsuId;              // Which RSU should forward this packet (if controller decided)
}
