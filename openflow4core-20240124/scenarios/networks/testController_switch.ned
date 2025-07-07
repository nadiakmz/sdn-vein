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

package openflow.scenarios.networks;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;
import openflow.hyperflow.Hyper_Flow_Synchronizer;
import openflow.openflow.controller.Open_Flow_Controller;
import openflow.openflow.switch.Open_Flow_Switch;
import openflow.utility.ControllerInvolvementFilter;
import openflow.utility.StaticSpanningTree;


//
// TODO auto-generated type
//
network TestController_switch
{
    parameters:
        int numClients @display("bgb=768,465") = default(1);
        int numControllers @display("bgb=768,465") = default(1);

        @display("bgb=925,638");
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

        controllerInvolvementFilter: ControllerInvolvementFilter {
            @display("p=59,39");
        }

//        hf_synchronizer: Hyper_Flow_Synchronizer if numControllers >=2 {
//            @display("p=366,39");
//        }

        clientG1[numClients]: StandardHost {
            @display("p=61,262");
        }


        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=61,175");
        }

        open_flow_switch1: Open_Flow_Switch {
            @display("p=206,306");
        }
        open_flow_switch2: Open_Flow_Switch {
            @display("p=238.82399,440.412");
        }


        etherSwitch: EtherSwitch {
            @display("p=389.052,252.948");
        }

        open_flow_controller1: Open_Flow_Controller if numControllers >=1 {
            @display("p=246,142");
        }

    connections allowunconnected:
        for i=0..(numClients)-1 {
            clientG1[i].ethg++ <--> ethernetline <--> open_flow_switch1.gateDataPlane++;
        }




        etherSwitch.ethg++ <--> ethernetline <--> open_flow_switch1.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> open_flow_switch2.gateControlPlane++;

        etherSwitch.ethg++ <--> ethernetline <--> open_flow_controller1.ethg++ if numControllers >=1;
//        etherSwitch.ethg++ <--> ethernetline <--> hf_synchronizer.ethg++ if numControllers >=2;


}
