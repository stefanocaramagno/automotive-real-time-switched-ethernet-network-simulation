#ifndef _RTN_01_ETHERMAC_H
#define _RTN_01_ETHERMAC_H

#include <omnetpp.h>
#include <vector>

using namespace omnetpp;

class EtherMAC : public cSimpleModule
{
  public:
    // --- OMNeT++ output signals ---
    static simsignal_t txQueueLenSignal;    // Signal to track the length of the transmission queue

  private:
    // --- MAC configuration and parameters ---
    typedef enum {
        TX_STATE_IDLE,  // No packet being transmitted
        TX_STATE_TX,    // Currently transmitting a packet
        TX_STATE_IFG    // Waiting for inter-frame gap
    } tx_state_t;

    tx_state_t txState = TX_STATE_IDLE; // Current transmission state
    cPacketQueue txQueue;               // Outgoing packet queue (priority by PCP if configured)
    cPacketQueue rxQueue;               // Incoming frames queued while another is being received
    cPacket* rxBuffer = nullptr;        // Packet currently being received

    uint64_t datarate;                      // Link speed in bits per second
    simtime_t ifgDuration = SIMTIME_ZERO;   // Inter-frame gap duration in seconds
    std::vector<int> vlans;                 // List of allowed VLAN IDs

  protected:
    // --- OMNeT++ module methods ---
    virtual void initialize() override;                 // Initialize module state and parameters
    virtual void handleMessage(cMessage *msg) override; // Handle incoming events and packets

    // --- Transmission & Reception helpers ---
    virtual void startTransmission();           // Begin transmission of next packet
    virtual void startReception(cPacket *pkt);  // Schedule reception of a packet
    virtual bool vlanFilter(cPacket *pkt);      // Filter packets by allowed VLANs

    // --- Priority queue comparator ---
    static int pcpComparator(cObject *a, cObject *b);   // Compare packets using PCP (priority)
};

#endif
