package org.car2x.veins.nodes;

import org.car2x.veins.base.modules.*;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.IPv4NetworkLayer;
import inet.networklayer.ipv4.IPv4RoutingTable;
import inet.transportlayer.udp.UDP;
import inet.linklayer.ethernet.EthernetInterface;

import org.car2x.veins.base.modules.IBaseApplLayer;
import org.car2x.veins.modules.nic.INic80211p;
import org.car2x.veins.modules.application.traci.TraCIDemoRSU11p;

module RSU
{
    parameters:
        @networkNode();
        string applType;
        @display("bgb=400,300;i=device/rsu_s;is=s");

    gates:
        input veinsradioIn @loose;
        inout ethg @loose;

    submodules:
        appl: TraCIDemoRSU11p {
            @display("p=200,50");
        }
        udp: UDP {
            parameters:
                interfaceTableModule = "^.interfaceTable";
                @display("p=200,125");
        }
        networkLayer: IPv4NetworkLayer {
            parameters:
                interfaceTableModule = "^.interfaceTable";
                routingTableModule = "^.routingTable";
                @display("p=200,200;q=queue");
        }
        interfaceTable: InterfaceTable {
            @display("p=350,125");
        }
        routingTable: IPv4RoutingTable {
             parameters:
                interfaceTableModule = "^.interfaceTable";
                @display("p=350,50");
        }
        eth: EthernetInterface {
            parameters:
                interfaceTableModule = "^.interfaceTable";
                @display("p=350,200");
        }
        nic: <"Nic80211p"> like INic80211p {
            @display("p=50,200");
        }
        // This explicitly creates the correct mobility module.
        mobility: BaseMobility {
            parameters:
                @display("p=130,172;i=block/cogwheel");
        }

    connections allowunconnected:
        // --- Veins wireless stack path ---
        nic.upperLayerOut --> appl.lowerLayerIn;
        appl.lowerLayerOut --> nic.upperLayerIn;
        appl.lowerControlOut --> nic.upperControlIn;
        appl.lowerControlIn <-- nic.upperControlOut;
        // --- INET wired stack path ---
        appl.socketOut --> udp.appIn++;
        udp.appOut++ --> appl.socketIn;
        networkLayer.transportOut++ --> udp.ipIn;
        udp.ipOut --> networkLayer.transportIn++;
        eth.upperLayerOut --> networkLayer.ifIn++;
        networkLayer.ifOut++ --> eth.upperLayerIn;
        
        // --- Physical Gate Connections ---
        eth.phys <--> ethg;
        veinsradioIn --> nic.radioIn;
}
