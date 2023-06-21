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
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

#define BaseBandwidth "1Mbps"
#define IncreasedBandwidth "10Mbps"


using namespace ns3;

int main(int argc, char* argv[]){
	Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue(true));
	NodeContainer nodes;
	nodes.Create(4);

	//instalacija steka protokola na cvorovima
	InternetStackHelper internet;
	internet.Install(nodes);

	//pravimo linkove
	PointToPointHelper p2p;
	p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("100p"));
	p2p.SetDeviceAttribute("DataRate", StringValue(BaseBandwidth));
	p2p.SetChannelAttribute("Delay", StringValue("5ms"));
	NetDeviceContainer n2n1 = p2p.Install(nodes.Get(2), nodes.Get(1));
	NetDeviceContainer n3n1 = p2p.Install(nodes.Get(3), nodes.Get(1));
	p2p.SetDeviceAttribute("DataRate", StringValue(IncreasedBandwidth));
	NetDeviceContainer n1n0 = p2p.Install(nodes.Get(1), nodes.Get(0));
	
	//dodela IP adresa
	Ipv4AddressHelper ip;
	
	ip.SetBase("10.0.0.0", "255.255.255.0");
	Ipv4InterfaceContainer i1i0 = ip.Assign(n1n0);

	ip.SetBase("10.0.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i2i1 = ip.Assign(n2n1);

	ip.SetBase("10.0.2.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i1 = ip.Assign(n3n1);

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	//izvor saobracaja
	uint16_t port = 9;
//	BulkSendHelper sender("ns3::TcpSocketFactory", InetSocketAddress(i1i0.GetAddress(1),port));
//	sender.SetAttribute("MaxBytes", UintegerValue(0));
	OnOffHelper sender("ns3::UdpSocketFactory", InetSocketAddress(i1i0.GetAddress(1),port));
	sender.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	sender.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	sender.SetAttribute("DataRate", StringValue(BaseBandwidth));
	sender.SetAttribute("PacketSize", UintegerValue(200));
	ApplicationContainer senderApps = sender.Install(NodeContainer(nodes.Get(2), nodes.Get(3)));
	senderApps.Start(Seconds(0.0));
	senderApps.Stop(Seconds(20.0));

	//prijem saobracaja
	PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(),port));
	ApplicationContainer sinkApps = sink.Install(nodes.Get(0));
	sinkApps.Start(Seconds(0.0));
	sinkApps.Stop(Seconds(20.0));


	//snimanje izvestaja
	AsciiTraceHelper ascii;
	Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("zagusenje.tr");
	p2p.EnableAsciiAll(stream);
	p2p.EnablePcapAll("zagusenje");

	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowMonitorHelper;
	flowMonitor = flowMonitorHelper.InstallAll();

	Simulator::Stop(Seconds(25.0));
	Simulator::Run();
	
	flowMonitor->SerializeToXmlFile("izlaz.xml",true,true);
	Simulator::Destroy();
}

