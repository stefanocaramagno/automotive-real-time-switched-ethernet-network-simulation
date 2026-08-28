package rtn_01;

simple PeriodicTrafficGen
{
    parameters:
        @display("i=block/app2");
        @statistic[E2EDelay](record=vector; checkSignals=false);
        @statistic[E2EBurstDelay](record=vector; checkSignals=false);
        @statistic[FrameLossRatio](record=max,vector; checkSignals=false);

        volatile double period @unit(s) = default(0s);
        double startTime @unit(s) = default(0s);
        string name;
        int payloadSize @unit(B) = default(0B);
        int burstSize = default(1);
        string destAddr = default("");
        string srcAddr;
        int vlanid = default(-1);
    	int pcp = default(-1);

    gates:
        input lowerLayerIn;
        output lowerLayerOut;
}
