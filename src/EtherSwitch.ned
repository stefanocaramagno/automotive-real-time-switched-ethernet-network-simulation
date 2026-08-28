package rtn_01;

module EtherSwitch
{
    parameters:
        int portCount;
        @display("bgb=298,325;i=device/modem");

    gates:
        input channelIn[portCount] @allowunconnected;
        output channelOut[portCount] @allowunconnected;

    submodules:
        eth[portCount]: EtherMAC {
            @display("p=199,166,r,30");
        }
        relayUnit: RelayUnit {
            parameters:
            	@display("p=72,166");
        }

    connections:
        for i=0..portCount-1 {
            channelIn[i] --> eth[i].channelIn;
            channelOut[i] <-- eth[i].channelOut;

            eth[i].upperLayerOut --> relayUnit.portGatesIn++;
            eth[i].upperLayerIn <-- relayUnit.portGatesOut++;
        }
}
