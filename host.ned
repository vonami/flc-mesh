//
//    Copyright (C) 2008    Dmitry Ivanov <vonami@gmail.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

import
    "NotificationBoard",
    "InterfaceTable",
    "RoutingTable",
    "OdsrNetworkLayer",
    "OdsrBursyGenerator",
    "Ieee80211NicAdhoc",
    "BasicMobility";

module Host
    parameters:
    gates:
        in: radioIn;
    submodules:
        notificationBoard: NotificationBoard;
            display: "p=130,300;i=block/control";
        interfaceTable: InterfaceTable;
            display: "p=130,200;i=block/table";
        mobility: NullMobility;
            display: "p=50,300;i=block/cogwheel";
        routingTable: RoutingTable;
            parameters:
                IPForward = true,
                routerId = "",
                routingFile = "";
            display: "p=50,200;i=block/table";
        generator: OdsrBurstyGenerator;
            display: "p=250,50;i=block/transport";
        odsr: OdsrLayer;
            display: "p=250,150;i=block/plug";
        networkLayer: OdsrNetworkLayer;
            parameters:
                proxyARP = false;
            gatesizes:
                ifIn[1],
                ifOut[1];
            display: "p=250,250;i=block/fork;q=queue";
        wlan: Ieee80211NicAdhoc;
            display: "p=250,350;q=queue;i=block/ifcard";
    connections nocheck:
        odsr.toNetwork --> networkLayer.ODSRIn;
        odsr.fromNetwork <-- networkLayer.ODSROut;

        odsr.toGenerator --> generator.fromOdsr;
        odsr.fromGenerator <-- generator.toOdsr;

        wlan.uppergateOut --> networkLayer.ifIn[0];
        wlan.uppergateIn <-- networkLayer.ifOut[0];

        radioIn --> wlan.radioIn;

    display: "b=310,390";
endmodule

