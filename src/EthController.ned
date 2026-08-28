package rtn_01;

module EthController
{
    @display("bgb=100,100");

    gates:
        input lowerLayerIn;
        output lowerLayerOut;
        input upperLayerIn;
        output upperLayerOut;

    submodules:
        mac: EtherMAC {
            @display("p=70,50");
        }
        encap: EthEncap {
            @display("p=30,50");
        }

    connections:
        lowerLayerIn --> mac.channelIn;
        lowerLayerOut <-- mac.channelOut;

        mac.upperLayerOut --> encap.lowerLayerIn;
        mac.upperLayerIn <-- encap.lowerLayerOut;

        encap.upperLayerOut --> upperLayerOut;
        encap.upperLayerIn <-- upperLayerIn;
}
