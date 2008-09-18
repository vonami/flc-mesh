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
    "IP",
    "ARP",
    "ICMP",
    "IGMP",
    "ErrorHandling";

module OdsrNetworkLayer
    parameters:
        proxyARP: bool;
    gates:
        in: ifIn[];
        in: TCPIn;
        in: UDPIn;
        in: ODSRIn;
        in: pingIn;
        out: ifOut[];
        out: TCPOut;
        out: UDPOut;
        out: ODSROut;
        out: pingOut;

    submodules:
        ip: IP;
            parameters:
                timeToLive = 32,
                multicastTimeToLive = 32,
                fragmentTimeout = 60,
                protocolMapping = "6:0,17:1,1:2,2:3,2525:4";
            gatesizes:
                transportIn[5],
                transportOut[5],
                queueIn[sizeof(ifIn)];
            display: "p=85,95;i=block/routing;q=queue";
        arp: ARP;
            parameters:
                proxyARP = proxyARP;
            gatesizes:
                nicOut[sizeof(ifOut)];
            display: "p=163,206;i=block/layer;q=pendingQueue";
        icmp: ICMP;
            display: "p=160,63;i=block/control_s";
        igmp: IGMP;
            display: "p=160,122;i=block/cogwheel_s";
        errorHandling: ErrorHandling;
            display: "p=239,63;i=block/process_s";
    connections nocheck:
        // transport Layer
        ip.transportOut[0] --> TCPOut;
        ip.transportIn[0] <-- TCPIn;

        ip.transportOut[1] --> UDPOut;
        ip.transportIn[1] <-- UDPIn;

        ip.transportOut[2] --> icmp.localIn;
        ip.transportIn[2] <-- icmp.sendOut;

        ip.transportOut[3] --> igmp.localIn;
        ip.transportIn[3] <-- igmp.sendOut;

        ip.transportOut[4] --> ODSROut;
        ip.transportIn[4] <-- ODSRIn;

        icmp.pingOut --> pingOut;
        icmp.pingIn <-- pingIn;

        icmp.errorOut --> errorHandling.in;

        ip.queueOut --> arp.ipIn;

        // L2 interfaces to IP and from ARP
        for i=0..sizeof(ifOut)-1 do
            ifIn[i] --> ip.queueIn[i] display "m=s";
            ifOut[i] <-- arp.nicOut[i] display "m=s";
        endfor;
endmodule

