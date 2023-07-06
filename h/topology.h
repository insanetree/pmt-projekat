#pragma once
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
#define QueueSize "1024p"
#define LinkDelay "5ms"
#define PacketSize 200

using namespace ns3;

void buildTopology(bool optimize);
