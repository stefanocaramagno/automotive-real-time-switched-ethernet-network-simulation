package rtn_01;

simple AppDispatcher
{
    parameters:
    	int appCount;
    	@display("i=block/classifier");
    	
    gates:
        output upperLayerOut[appCount];
        input upperLayerIn[appCount];
        output lowerLayerOut;
        input lowerLayerIn;
}