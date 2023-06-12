#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

int main(int argc, char* argv[]){
	ns3::Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", ns3::BooleanValue(true));
	ns3::NodeContainer nodes;
	nodes.Create(4);

	//instalacija steka protokola na cvorovima
	ns3::InternetStackHelper internet;
	internet.Install(nodes);

	//pravimo linkove
	ns3::PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", ns3::StringValue("10Mbps"));
	p2p.SetChannelAttribute("Delay", ns3::StringValue("5ms"));
	ns3::NetDeviceContainer n2n1 = p2p.Install(nodes.Get(2), nodes.Get(1));
	ns3::NetDeviceContainer n3n1 = p2p.Install(nodes.Get(3), nodes.Get(1));
	ns3::NetDeviceContainer n1n0 = p2p.Install(nodes.Get(1), nodes.Get(0));
	
	//dodela IP adresa
	ns3::Ipv4AddressHelper ip;
	
	ip.SetBase("10.0.0.0", "255.255.255.0");
	ns3::Ipv4InterfaceContainer i1i0 = ip.Assign(n1n0);

	ip.SetBase("10.0.1.0", "255.255.255.0");
	ns3::Ipv4InterfaceContainer i2i1 = ip.Assign(n2n1);

	ip.SetBase("10.0.2.0", "255.255.255.0");
	ns3::Ipv4InterfaceContainer i3i1 = ip.Assign(n3n1);

	ns3::Ipv4GlobalRoutingHelper::PopulateRoutingTables();
}