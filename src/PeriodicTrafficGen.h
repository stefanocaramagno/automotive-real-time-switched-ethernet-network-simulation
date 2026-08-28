#ifndef __RTN_01_PERIODICTRAFFICGEN_H_
#define __RTN_01_PERIODICTRAFFICGEN_H_

#include <omnetpp.h>

using namespace omnetpp;
using namespace std;

// Data structure used to uniquely identify a packet
struct PacketId {
  int pktNumber;        // Sequential packet number
  int burstSize;        // Burst size to which the packet belongs
  simtime_t genTime;    // Packet generation time

  // Comparison operator for use in map
  bool operator<(const PacketId& other) const {
      if (pktNumber != other.pktNumber)
          return pktNumber < other.pktNumber;
      if (burstSize != other.burstSize)
          return burstSize < other.burstSize;
      return genTime < other.genTime;
  }

  // Equality operator for additional checks if needed
  bool operator==(const PacketId& other) const {
      return (pktNumber == other.pktNumber &&
              burstSize == other.burstSize &&
              genTime == other.genTime);
  }
};

class PeriodicTrafficGen : public cSimpleModule {
  public:
    // --- OMNeT++ output signals ---
    static simsignal_t e2eDelaySignal;      // Signal to track End-to-End delay of individual fragments
    static simsignal_t e2eBurstDelaySignal; // Signal to track End-to-End delay of an entire burst
    static simsignal_t flrSignal;           // Signal to track Frame-to-Loss-Ratio over time

  private:
    // Track received packets (number + genTime)
    std::set<std::pair<int, simtime_t>> receivedPkts;
    // Map of received packets (pktNumber -> genTime)
    std::map<PacketId, simtime_t> receivedPackets;

    // Frame counters
    long rxFrameCount = 0;
    long txFrameCount = 0;

  protected:
    // --- OMNeT++ module methods ---
    virtual void initialize() override;                 // Initialize module state and parameters
    virtual void handleMessage(cMessage *msg) override; // Handle incoming events and packets

    // --- Packet generation ---
    virtual void transmitPacket();  // Create and send packets

    // --- Traffic generator configuration ---
    simtime_t period;               // Interval between packet generations
    simtime_t startTime;            // Simulation time when traffic starts
    string name;                    // Application name/identifier
    unsigned long long payloadSize; // Size of each application packet payload (in bytes)
    unsigned int burstSize;         // Number of fragments per burst
    string destAddr;                // Destination MAC address
    string srcAddr;                 // Source MAC address
    int vlanid;                     // VLAN identifier
    int pcp;                        // Priority Code Point

    // Transmission frame counter tracking variables
    int lastCountedPkt = 0;             // Last packet index counted in burst
    simtime_t currentBurstGenTime = -1; // Generation time of current burst

    double flr; // Frame-to-Loss-Ratio
};

#endif
