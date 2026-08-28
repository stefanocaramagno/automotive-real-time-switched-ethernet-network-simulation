#ifndef __RTN_01_ETHENCAP_H_
#define __RTN_01_ETHENCAP_H_

#include <omnetpp.h>

using namespace omnetpp;
using namespace std;

class EthEncap : public cSimpleModule
{
  protected:
    // --- OMNeT++ module methods ---
    virtual void initialize() override;                 // Initialize module parameter
    virtual void handleMessage(cMessage *msg) override; // Handle incoming messages

    // --- Encapsulation configuration and parameters ---
    string address; // MAC address
};

#endif
