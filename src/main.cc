#include "topology.h"

int main(int argc, char* argv[]){
	Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue(true));
	
	buildTopology(false);
	buildTopology(true);

}

