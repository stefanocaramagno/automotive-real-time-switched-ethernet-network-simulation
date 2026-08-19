#include "RelayUnit.h"
#include "EthernetFrame_m.h"

Define_Module(RelayUnit);

void RelayUnit::initialize()
{
    // Initialization logic (currently unused)
}

void RelayUnit::handleMessage(cMessage *msg)
{
    int arrivalGateIndex = msg->getArrivalGate()->getIndex();

    for (int i = 0; i < gateSize("portGatesOut"); i++) {
        if (i != arrivalGateIndex) {
            // Forward duplicate frame to all other ports
            send(msg->dup(), "portGatesOut", i);
        }
    }

    delete msg;
}
