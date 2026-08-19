package rtn_01;

simple EtherMAC {
    parameters:
    	@display("i=block/mac");
    	@signal[txQueueLen](type=int);
    	@statistic[maxTxQueueLen](source=txQueueLen; record=max; checkSignals=false);
    	
    	int datarate @unit(bps) = default(10Gbps);
    	object vlans = default([]);
    	
    gates:
        input upperLayerIn;
        output upperLayerOut;
        input channelIn;
        output channelOut;
}
