#include "AppDispatcher.h"

Define_Module(AppDispatcher);

void AppDispatcher::initialize()
{
    // Initialization logic (currently unused)
}

//
// Dispatches messages between the network (lower layer) and application (upper layer)
//
// If message comes from network, forward a duplicate
// to all upperLayerOut gates of vector gate
//
// If message comes from an application, forward it
// to lowerLayerOut
//
void AppDispatcher::handleMessage(cMessage *msg)
{
    // Message received from network (lower layer)
    if(msg->getArrivalGate() == gate("lowerLayerIn")) {
        for(int i = 0; i < gateSize("upperLayerOut"); ++i) {
            cMessage *duplicate = msg->dup();
            send(duplicate, "upperLayerOut", i);
        }
        delete msg; // Original message no longer needed
        return;
    }

    // Message received from an application (upper layer)
    send(msg, "lowerLayerOut");
    return;
}
