#!/bin/sh

ROOT=~/phd/INET-20061020

CONTRACT_INCLUDES="-I${ROOT}/Transport/Contract -I${ROOT}/Network/Contract -I${ROOT}/NetworkInterfaces/Contract -I${ROOT}/Base -I${ROOT}/Util"

#ALL_INET_INCLUDES="${CONTRACT_INCLUDES} -I${ROOT}/Network/IPv4 -I${ROOT}/Network/AutoRouting -I${ROOT}/Network/Queue -I${ROOT}/Network/Quagga -I${ROOT}/Network/OSPFv2 -I${ROOT}/Network/OSPFv2/Interface -I${ROOT}/Network/OSPFv2/MessageHandler -I${ROOT}/Network/OSPFv2/Neighbor -I${ROOT}/Network/OSPFv2/Router -I${ROOT}/Transport/TCP -I${ROOT}/Transport/TCP/flavours -I${ROOT}/Transport/TCP/queues -I${ROOT}/Transport/UDP -I${ROOT}/NetworkInterfaces -I${ROOT}/Network/ARP -I${ROOT}/NetworkInterfaces/Ethernet -I${ROOT}/NetworkInterfaces/EtherSwitch -I${ROOT}/NetworkInterfaces/PPP -I${ROOT}/Applications/Generic -I${ROOT}/Applications/Ethernet -I${ROOT}/Applications/TCPApp -I${ROOT}/Applications/UDPApp -I${ROOT}/Applications/PingApp -I${ROOT}/Util/HeaderSerializers -I${ROOT}/Nodes/INET -I${ROOT}/Nodes/Wireless -I${ROOT}/Nodes/Adhoc"



echo ${ALL_INET_INCLUDES}

#opp_makemake -r -f -N -s $ALL_INET_INCLUDES
opp_makemake -u Cmdenv -f -x -w -N -I. -I$ROOT/Base -I$ROOT/Util -I$ROOT/Obsolete/IPv4d -I$ROOT/Obsolete/LDP -I$ROOT/Obsolete/RSVP_TE -I$ROOT/Obsolete/RSVP -I$ROOT/Obsolete/Socket -I$ROOT/Obsolete/TcpModule -I$ROOT/NetworkInterfaces/PPP -I$ROOT/NetworkInterfaces/EtherSwitch -I$ROOT/NetworkInterfaces/MFCore -I$ROOT/NetworkInterfaces/Ethernet -I$ROOT/NetworkInterfaces/Contract -I$ROOT/NetworkInterfaces/MF80211/phyLayer/decider -I$ROOT/NetworkInterfaces/MF80211/phyLayer/snrEval -I$ROOT/NetworkInterfaces/MF80211/macLayer -I$ROOT/NetworkInterfaces/Ieee80211/Mac -I$ROOT/NetworkInterfaces/Ieee80211/Mgmt -I$ROOT/NetworkInterfaces/Radio -I$ROOT/Transport/TCP -I$ROOT/Transport/TCP/queues -I$ROOT/Transport/TCP/flavours -I$ROOT/Transport/Contract -I$ROOT/Transport/UDP -I$ROOT/Transport/RTP -I$ROOT/Transport/RTP/tmp -I$ROOT/Transport/RTP/Profiles/AVProfile -I$ROOT/Mobility -I$ROOT/Network/LDP -I$ROOT/Network/IPv6 -I$ROOT/Network/RSVP_TE -I$ROOT/Network/Contract -I$ROOT/Network/Queue -I$ROOT/Network/Extras -I$ROOT/Network/IPv4 -I$ROOT/Network/ICMPv6 -I$ROOT/Network/AutoRouting -I$ROOT/Network/Quagga -I$ROOT/Network/Quagga/quaggasrc/quagga -I$ROOT/Network/Quagga/quaggasrc/quagga/ospfd -I$ROOT/Network/Quagga/quaggasrc/quagga/ripd -I$ROOT/Network/Quagga/quaggasrc/quagga/lib -I$ROOT/Network/Quagga/quaggasrc/quagga/zebra -I$ROOT/Network/MPLS -I$ROOT/Network/OSPFv2/Interface -I$ROOT/Network/OSPFv2/MessageHandler -I$ROOT/Network/OSPFv2 -I$ROOT/Network/OSPFv2/Router -I$ROOT/Network/OSPFv2/Neighbor -I$ROOT/Network/ARP -I$ROOT/Network/TED -I$ROOT/Applications/UDPApp -I$ROOT/Applications/TCPApp -I$ROOT/Applications/Generic -I$ROOT/Applications/Ethernet -I$ROOT/Applications/PingApp -I$ROOT/World
