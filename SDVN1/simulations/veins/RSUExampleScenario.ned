//
// Copyright (C) 2017 Christoph Sommer <sommer@ccs-labs.org>
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

package veins;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;
import openflow.hyperflow.Hyper_Flow_Synchronizer;
import openflow.openflow.controller.Open_Flow_Controller;
import openflow.openflow.switch.Open_Flow_Switch;
import openflow.utility.ControllerInvolvementFilter;
import openflow.utility.StaticSpanningTree;

import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Scenario;

import sdvn.Visualizer;

network RSUExampleScenario extends Scenario
{
    parameters:
        int numClients @display("bgb=768,465") = default(1);
        int numControllers @display("bgb=768,465") = default(1);
        int numRSUs = default(2); // Example number of RSUs

        //@display("bgb=925,638");
        @display("bgb=828,561");
    types:
        channel ethernetline extends ThruputMeteringChannel
        {
            delay = 1us;
            datarate = 100Mbps;
            thruputDisplayFormat = "u";
        }

    submodules:
        spanningTree: StaticSpanningTree {
            @display("p=61,108");
        }
        visualizer: Visualizer { // <-- Add this submodule
            @display("p=100,100;i=block/camera");
        }
        controllerInvolvementFilter: ControllerInvolvementFilter {
            @display("p=59,39");
        }
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=61,175");
        }

        etherSwitch: EtherSwitch {
            @display("p=389.052,252.948");
        }

        open_flow_controller_main: Open_Flow_Controller {
            @display("p=388.72,140.12");
        }

        open_flow_controller1: Open_Flow_Controller if numControllers >=2 {
            @display("p=137.86,181.93");
        }
        open_flow_controller2: Open_Flow_Controller if numControllers >=3 {
            @display("p=59.89,288.15");
        }
        open_flow_controller3: Open_Flow_Controller if numControllers >=4 {
            @display("p=708.51,318.66");
        }
        open_flow_controller4: Open_Flow_Controller if numControllers >=5 {
            @display("p=675.74,216.95999");
        }

        open_flow_switch1: Open_Flow_Switch {
            @display("p=29.38,429.4;i=device/switch");
        }
        open_flow_switch2: Open_Flow_Switch {
            @display("p=239.56,436.18;i=device/switch");
        }
        open_flow_switch3: Open_Flow_Switch {
            @display("p=675.74,436.18;i=device/switch");
        }
        open_flow_switch4: Open_Flow_Switch {
            @display("p=496.07,436.18;i=device/switch");
        }
        rsu[12]: RSU {
            @display("p=388.72,528.83997;i=veins/sign/yellowdiamond;is=vs");
            //@display("p=150,140;r=300;b=600,600,oval,#808080,,0");
        }
//		rsu[1]: RSU {
//            @display("p=150,140");
//
//        }

    connections allowunconnected:
        // Connect each RSU's Ethernet gate to a data plane gate of the OpenFlow Switch
        //        // The 'gateDataPlane' gates of the Open_Flow_Switch are used for data plane connections to nodes like RSUs.
        //        for i=0..numRSUs-1 {
        //            // This requires gateDataPlane to be a vector gate in Open_Flow_Switch, which it is.
        //            rsu[i].ethg <--> ethernetline <--> open_flow_switch1.gateDataPlane++;
        //        }
        //       
        rsu[0].ethg <--> ethernetline <--> open_flow_switch4.gateDataPlane++;
        rsu[1].ethg <--> ethernetline <--> open_flow_switch4.gateDataPlane++;
        rsu[2].ethg <--> ethernetline <--> open_flow_switch4.gateDataPlane++;

        rsu[3].ethg <--> ethernetline <--> open_flow_switch3.gateDataPlane++;
        rsu[4].ethg <--> ethernetline <--> open_flow_switch4.gateDataPlane++;
        rsu[5].ethg <--> ethernetline <--> open_flow_switch3.gateDataPlane++;

        rsu[6].ethg <--> ethernetline <--> open_flow_switch3.gateDataPlane++;
        rsu[7].ethg <--> ethernetline <--> open_flow_switch4.gateDataPlane++;
        rsu[8].ethg <--> ethernetline <--> open_flow_switch3.gateDataPlane++;

        rsu[9].ethg <--> ethernetline <--> open_flow_switch2.gateDataPlane++;
        rsu[10].ethg <--> ethernetline <--> open_flow_switch1.gateDataPlane++;
        rsu[11].ethg <--> ethernetline <--> open_flow_switch2.gateDataPlane++;

        etherSwitch.ethg++ <--> ethernetline <--> open_flow_switch1.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> open_flow_switch2.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> open_flow_switch3.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> open_flow_switch4.gateControlPlane++;

        etherSwitch.ethg++ <--> ethernetline <--> open_flow_controller_main.ethg++;
        etherSwitch.ethg++ <--> ethernetline <--> open_flow_controller1.ethg++ if numControllers >=2;
        etherSwitch.ethg++ <--> ethernetline <--> open_flow_controller2.ethg++ if numControllers >=3;
        etherSwitch.ethg++ <--> ethernetline <--> open_flow_controller3.ethg++ if numControllers >=4;
        etherSwitch.ethg++ <--> ethernetline <--> open_flow_controller4.ethg++ if numControllers >=5;

}

