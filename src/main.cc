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
	p2p.SetQueue("ns3::DropTailQueue", "MaxSize", ns3::StringValue("50p"));
	p2p.SetDeviceAttribute("DataRate", ns3::StringValue("100kbps"));
	p2p.SetChannelAttribute("Delay", ns3::StringValue("5ms"));
	ns3::NetDeviceContainer n2n1 = p2p.Install(nodes.Get(2), nodes.Get(1));
	ns3::NetDeviceContainer n3n1 = p2p.Install(nodes.Get(3), nodes.Get(1));
	p2p.SetDeviceAttribute("DataRate", ns3::StringValue("1Mbps"));
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

	//izvor saobracaja
	uint16_t port = 9;
	ns3::BulkSendHelper sender("ns3::TcpSocketFactory", ns3::InetSocketAddress(i1i0.GetAddress(1),port));
	sender.SetAttribute("MaxBytes", ns3::UintegerValue(0));
	ns3::ApplicationContainer senderApps = sender.Install(ns3::NodeContainer(nodes.Get(2), nodes.Get(3)));
	senderApps.Start(ns3::Seconds(0.0));
	senderApps.Stop(ns3::Seconds(20.0));

	//prijem saobracaja
	ns3::PacketSinkHelper sink("ns3::TcpSocketFactory", ns3::InetSocketAddress(ns3::Ipv4Address::GetAny(),port));
	ns3::ApplicationContainer sinkApps = sink.Install(nodes.Get(0));
	sinkApps.Start(ns3::Seconds(0.0));
	sinkApps.Stop(ns3::Seconds(20.0));


	//snimanje izvestaja
	ns3::AsciiTraceHelper ascii;
	ns3::Ptr<ns3::OutputStreamWrapper> stream = ascii.CreateFileStream("zagusenje.tr");
	p2p.EnableAsciiAll(stream);
	p2p.EnablePcapAll("zagusenje");

	ns3::Simulator::Stop(ns3::Seconds(20.0));
	ns3::Simulator::Run();
	ns3::Simulator::Destroy();
}

