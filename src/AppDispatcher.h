#ifndef __RTN_01_APPDISPATCHER_H_
#define __RTN_01_APPDISPATCHER_H_

#include <omnetpp.h>

using namespace omnetpp;

class AppDispatcher : public cSimpleModule
{
  protected:
    // --- OMNeT++ module methods ---
    virtual void initialize() override;                 // Initialize module (currently unused)
    virtual void handleMessage(cMessage *msg) override; // Handle incoming messages
};

#endif
