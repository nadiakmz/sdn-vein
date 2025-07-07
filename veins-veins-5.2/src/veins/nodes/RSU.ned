package org.car2x.veins.nodes;

import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.IPv4NetworkLayer;
import inet.networklayer.ipv4.IPv4RoutingTable;
import inet.transportlayer.udp.UDP;
import inet.linklayer.ethernet.EthernetInterface;
import inet.mobility.static.StationaryMobility; // Required for static positions

import org.car2x.veins.base.modules.IBaseApplLayer;
import org.car2x.veins.modules.nic.INic80211p;

module RSU
{
    parameters:
        @networkNode();
        string applType;
        @display("bgb=400,300;i=device/rsu_s;is=s");

    gates:
        input veinsradioIn @loose;
        inout ethg;

    submodules:
        appl: <applType> like IBaseApplLayer {
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
        mobility: StationaryMobility {
            @display("p=50,125;i=block/cogwheel");
        }

    connections allowunconnected:
        // All necessary connections for data and control
        udp.appOut++ --> appl.lowerLayerIn;
        udp.appIn++ <-- appl.lowerLayerOut;
        appl.lowerControlOut --> nic.upperControlIn;
        appl.lowerControlIn <-- nic.upperControlOut;
        networkLayer.transportOut++ --> udp.ipIn;
        networkLayer.transportIn++ <-- udp.ipOut;
        nic.upperLayerOut --> networkLayer.ifIn++;
        networkLayer.ifOut++ --> nic.upperLayerIn;
        eth.upperLayerOut --> networkLayer.ifIn++;
        networkLayer.ifOut++ --> eth.upperLayerIn;
        eth.phys <--> ethg;
        veinsradioIn --> nic.radioIn;
}