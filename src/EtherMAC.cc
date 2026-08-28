#include "EtherMAC.h"
#include "EthernetFrame_m.h"

// Register signal to retrieve statistic
simsignal_t EtherMAC::txQueueLenSignal = registerSignal("txQueueLen");

Define_Module(EtherMAC);

//
// Initialization of MAC module
//
void EtherMAC::initialize()
{
    // MAC starts idle
    txState = TX_STATE_IDLE;
    rxBuffer = nullptr;

    // Read link speed from parameter
    datarate = par("datarate");

    // Name and optional comparator for the transmission queue (priority queue)
    txQueue = cPacketQueue("TransmissionQueue", pcpComparator);
    rxQueue = cPacketQueue("ReceptionQueue", pcpComparator);

    // Inter-frame gap duration (96 bits at link speed)
    ifgDuration = 96.0 /(double) datarate;

    // Read allowed VLAN IDs from parameter
    cValueArray *vlanArray = check_and_cast<cValueArray*>(par("vlans").objectValue());
    for (int i = 0; i < vlanArray->size(); ++i) {
        vlans.push_back((int)vlanArray->get(i).intValue());
    }

    // Set maxTxQueueLen statistic equal to zero at the beginning
    emit(txQueueLenSignal, txQueue.getLength());
}

//
// Handles incoming messages, including self-messages (timers) and packets from gates
//
void EtherMAC::handleMessage(cMessage *msg)
{
    // Self-messages (internal timers)
    if (msg->isSelfMessage()) {
        if (strcmp(msg->getName(), "TxTimer") == 0) {
            delete msg;

            // Schedule inter-frame gap timer
            cMessage *ifgtim = new cMessage("IFGTimer");
            scheduleAt(simTime() + ifgDuration, ifgtim);
            txState = TX_STATE_IFG;
            return;

        } else if (strcmp(msg->getName(), "IFGTimer") == 0) {
            delete msg;

            startTransmission();
            return;

        } else if (strcmp(msg->getName(), "RxTimer") == 0) {
            delete msg;

            // Deliver or discard frame in rxBuffer
            if (rxBuffer != nullptr) {
                // Check for any bit error
                if (rxBuffer->hasBitError()) {
                    EV_DEBUG << "Received frame has bit errors --> dropped.\n";
                    delete rxBuffer;
                } else {
                    EV_DEBUG << "Frame received successfully --> sending to upper layer.\n";
                    send(rxBuffer, "upperLayerOut");
                }
                rxBuffer = nullptr;
            }

            // If there are queued packets, start receiving the next one
            if(!rxQueue.isEmpty()) {
                cPacket *nextPkt = check_and_cast<cPacket*>(rxQueue.pop());
                EV_DEBUG << "Dequeuing next frame for reception from rxQueue.\n";
                startReception(nextPkt);
            }
            return;
        }

        EV_DEBUG << "Unknown self-message: " << msg->getName() << "\n";
        delete msg;
        return;
    }

    cPacket *pkt = check_and_cast<cPacket *>(msg);


    // Packet from upper layer (RelayUnit)
    if(pkt->getArrivalGate() == gate("upperLayerIn")) {

        // Drop packet if VLAN is not allowed
        if(vlanFilter(pkt)) {
            EV_INFO << "VLAN filter --> dropping packet with VLAN ID not allowed.\n";
            delete msg;
            return;
        }
        EV_INFO << "VLAN filter --> packet with VLAN ID is allowed.\n";

        // Insert packet in transmission queue
        txQueue.insert(pkt);
        emit(txQueueLenSignal, txQueue.getLength());

        // Starts transmission if MAC is idle
        if(txState == TX_STATE_IDLE) {
            startTransmission();
        }

        return;
    }


    // Packet from channelIn
    if(rxBuffer != nullptr) {
        EV_DEBUG << "Already receiving a frame --> queueing incoming packet.\n";
        rxQueue.insert(pkt);
    } else {
        startReception(pkt);
    }
}

//
// Filters packets based on VLAN IDs
//
bool EtherMAC::vlanFilter(cPacket *pkt)
{
    if(vlans.size() == 0) {
        EV_DEBUG << "No VLAN filtering configured --> discard all packets.\n";
        return true;
    }

    EthernetQFrame *qf = dynamic_cast<EthernetQFrame *>(pkt);
    if(qf == nullptr) {
        EV_DEBUG << "Packet has no VLAN tag --> discarding.\n";
        return true;
    }

    int pktVlan = qf->getVlanid();
    for(int allowed : vlans) {
        if(allowed == pktVlan) {
            EV_DEBUG << "Packet accepted --> VLAN ID " << pktVlan << " is allowed.\n";
            return false;
        }
    }

    EV_DEBUG << "Packet dropped --> VLAN ID " << pktVlan << " is not in allowed list.\n";
    return true;
}

//
// Comparator function to implement a priority queue
//
int EtherMAC::pcpComparator(cObject *a, cObject *b)
{
    // Helper to extract PCP from EthernetQFrame
    auto getPcp = [](cObject *obj) -> int {
        EthernetQFrame *qFrame = check_and_cast<EthernetQFrame *>(obj);
        return qFrame->getPcp();
    };

    int pcpA = getPcp(a);
    int pcpB = getPcp(b);

    cPacket *pa = check_and_cast<cPacket*>(a);
    cPacket *pb = check_and_cast<cPacket*>(b);

    // Compare PCP first
    if (pcpA != pcpB)
        return (pcpA > pcpB) ? -1 : 1;

    // If PCPs are equal, use FIFO order based on creation time
    if (pa->getCreationTime() < pb->getCreationTime()) return -1;
    if (pa->getCreationTime() > pb->getCreationTime()) return 1;

    // Returns 0 when both priority and creation time are equal
    return 0;
}

//
// Start reception of an incoming packet
//
void EtherMAC::startReception(cPacket *pkt)
{
    rxBuffer = pkt;
    simtime_t rxdur = (double)pkt->getBitLength() / (double)datarate;
    EV_INFO << "Starting reception: " << pkt->getBitLength()
            << " bits, duration=" << rxdur << "s.\n";
    scheduleAt(simTime() + rxdur, new cMessage("RxTimer"));
}

//
// Start transmission of next packet in transmission queue
//
void EtherMAC::startTransmission()
{
    if(txQueue.getLength() == 0) {
        txState = TX_STATE_IDLE;
        return;
    }

    // Pop packet from priority queue
    cPacket *pkt = txQueue.pop();

    // Compute transmission duration
    simtime_t txDuration = (double)pkt->getBitLength() / (double)datarate;

    // Send packet and schedule end of transmission
    send(pkt, "channelOut");
    cMessage *txTimer = new cMessage("TxTimer");
    scheduleAt(simTime() + txDuration, txTimer);
    txState = TX_STATE_TX;
}
