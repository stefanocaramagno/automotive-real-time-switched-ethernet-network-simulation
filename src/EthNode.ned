package rtn_01;

module EthNode
{
    parameters:
        int appCount;
        @display("bgb=150,100,,,0;i=block/app2,,0");

    gates:
        input channelIn;
        output channelOut;

    submodules:
        controller: EthController {
            @display("p=120,50");
        }
        dispatcher: AppDispatcher {
            parameters:
            	@display("p=75,50");
            	appCount = parent.appCount;
        }
        app[appCount]: PeriodicTrafficGen {
            @display("p=30,50");
        }
    connections:
        channelIn --> controller.lowerLayerIn;
        channelOut <-- controller.lowerLayerOut;

        controller.upperLayerOut --> dispatcher.lowerLayerIn;
        controller.upperLayerIn <-- dispatcher.lowerLayerOut;

        for i=0..appCount - 1 {
            dispatcher.upperLayerOut[i] --> app[i].lowerLayerIn;
            dispatcher.upperLayerIn[i] <-- app[i].lowerLayerOut;
        }
}