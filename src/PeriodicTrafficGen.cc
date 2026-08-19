#include "PeriodicTrafficGen.h"
#include "AppPackets_m.h"
#include "EthernetFrame_m.h"

// Register signals to retrieve statistics
simsignal_t PeriodicTrafficGen::flrSignal = registerSignal("FrameLossRatio");
simsignal_t PeriodicTrafficGen::e2eDelaySignal = registerSignal("E2EDelay");
simsignal_t PeriodicTrafficGen::e2eBurstDelaySignal = registerSignal("E2EBurstDelay");

Define_Module(PeriodicTrafficGen);

//
// Initialization of application module
//
void PeriodicTrafficGen::initialize()
{
    // Application name
    name = par("name").str();

    // Source and destination MAC address
    srcAddr = par("srcAddr").str();
    destAddr = par("destAddr").str();

    // Time to send the first packet
    startTime = par("startTime");
    // Period between bursts
    period = par("period");

    // Total payload size to transmit
    payloadSize = par("payloadSize");
    // Number of fragment per burst
    burstSize = par("burstSize");

    // VLAN identifier
    vlanid = par("vlanid");
    // Priority code point
    pcp = par("pcp");

    // Schedule packet transmission if startTime is greater than zero
    if(startTime > 0) {
        cMessage *timer = new cMessage("TxTimer");
        scheduleAt(startTime, timer);
    }

    // Print configuration parameters
    EV_INFO << "PeriodicTrafficGen initialized with values:\n"
              << "name: " << name
              << ", srcAddr: " << srcAddr
              << ", destAddr: " << destAddr
              << ", startTime: " << startTime
              << ", period: " << period
              << ", payloadSize: " << payloadSize
              << ", burstSize: " << burstSize
              << ", vlanid: " << vlanid
              << ", pcp: " << pcp
              << endl;
}

//
// Handles incoming messages, including self-messages (timers) and packets from lower layer
//
void PeriodicTrafficGen::handleMessage(cMessage *msg)
{
    // Self-messages (internal timers)
    if(msg->isSelfMessage()) {
        if(strcmp(msg->getName(), "TxTimer") == 0) {
            transmitPacket();
            scheduleAt(simTime()+par("period"), msg);
            return;
        }
        error("Unexpected self-message received");
    }

    // Packet from lower layer
    DataPacket *pkt = check_and_cast<DataPacket *>(msg);

    // Drop packet not addressed to this node
    if(strncmp(pkt->getName(), name.c_str(), (size_t)name.size()) != 0) {
        delete pkt;
        return;
    }

    // Create packet identifier using packet number and burst size
    PacketId pktId;
    pktId.pktNumber = pkt->getPktNumber();
    pktId.burstSize = pkt->getBurstSize();
    pktId.genTime = pkt->getGenTime();

    // Check for duplicates
    auto it = receivedPackets.find(pktId);
    if(it != receivedPackets.end()) {
        // Duplicate detected
        EV_DEBUG << "DUPLICATE PACKET DETECTED: packet no. " << pkt->getPktNumber()
                << " of burst size " << pkt->getBurstSize()
                << " with genTime " << pkt->getGenTime() << endl;

        delete pkt;
        return;
    }

    // Store packet information for duplicate detection
    receivedPackets[pktId] = simTime();


    // Detect new burst based on genTime
    if (currentBurstGenTime < SIMTIME_ZERO || pkt->getGenTime() != currentBurstGenTime) {
        // New burst detected
        currentBurstGenTime = pkt->getGenTime();
        lastCountedPkt = 0;  // Reset counter for new burst
        EV_DEBUG << "New burst detected (genTime=" << currentBurstGenTime
                << "), resetting lastCountedPkt" << endl;
    }

    // Update txFrameCount based on packet number
    int pktNum = pkt->getPktNumber();
    if (pktNum > lastCountedPkt) {
        // Count the frames from lastCountedPkt+1 to pktNum
        int framesToAdd = pktNum - lastCountedPkt;
        txFrameCount += framesToAdd;
        lastCountedPkt = pktNum;

        EV_DEBUG << "TxFrameCount updated: added " << framesToAdd
                << " frames, total=" << txFrameCount
                << " (packet " << pktNum << "/" << pkt->getBurstSize() << ")" << endl;
    } else {
        EV_DEBUG << "Packet " << pktNum
                << " ignored for txFrameCount (already counted up to "
                << lastCountedPkt << ")" << endl;
    }


    // Compute end-to-end delay
    EV_DEBUG << "Received packet no. " << pkt->getPktNumber()
            << " of burst size " << pkt->getBurstSize() << endl;

    // Update received frames counter and emit
    rxFrameCount++;
    if (txFrameCount > 0) {
        flr = 1.0 - ((double)rxFrameCount / (double)txFrameCount);
        emit(flrSignal, flr);
    }

    simtime_t delay = simTime() - pkt->getGenTime();
    emit(e2eDelaySignal, delay);


    // Takes track of the end to end delay of the entire burst
    if(pkt->getPktNumber() == pkt->getBurstSize()) {
        emit(e2eBurstDelaySignal, delay);
    }

    delete pkt;
}

//
// Transmit packets while handling fragmentation
//
void PeriodicTrafficGen::transmitPacket() {
    // Total size of the original message
    int totSize = payloadSize;

    // Compute the basic size of each fragment
    int fragSize = ceil((double)totSize / burstSize);

    for(int i = 0; i < burstSize; i++) {
        // Last fragment might be smaller than previous ones
        int currSize = std::min(fragSize, totSize - (i * fragSize));

        // Create fragment
        DataPacket *frag = new DataPacket((name + " (fragment)").c_str());
        frag->setByteLength(currSize);
        frag->setGenTime(simTime());
        frag->setBurstSize(burstSize);
        frag->setPktNumber(i + 1);

        // Add Ethernet control information
        EthTransmitReq *req = new EthTransmitReq();
        req->setSrc(srcAddr.c_str());
        req->setDst(destAddr.c_str());
        req->setVlanid(vlanid);
        req->setPcp(pcp);
        frag->setControlInfo(req);

        // Send fragment to lower layer
        send(frag, "lowerLayerOut");
    }
}
