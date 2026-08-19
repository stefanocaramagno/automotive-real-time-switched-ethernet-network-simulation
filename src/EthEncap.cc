#include "EthEncap.h"
#include "EthernetFrame_m.h"

Define_Module(EthEncap);

//
// Initialization of encapsulation module
//
void EthEncap::initialize()
{
    // Read node MAC address from module parameter
    address = par("address").str();
}

//
// Handles messages arriving from both upper layer (dispatcher) and lower layer (MAC)
//
void EthEncap::handleMessage(cMessage *msg)
{
    // Message from upper layers (application / dispatcher)
    if(msg->getArrivalGate() == gate("upperLayerIn")) {

        // Extract control info from application message
        EthTransmitReq *request = dynamic_cast<EthTransmitReq *>(msg->removeControlInfo());
        if(request == nullptr) {
            error("Application message requires EthTransmitReq control info");
        }

        // Create a new Ethernet IEEE 802.1Q frame (with VLAN ID and PCP support)
        EthernetQFrame *frame = new EthernetQFrame();
        frame->setSrc(request->getSrc());       // Source MAC address
        frame->setDst(request->getDst());       // Destination MAC address
        frame->setVlanid(request->getVlanid()); // VLAN ID
        frame->setPcp(request->getPcp());       // Priority Code Point

        // Encapsulate application payload inside Ethernet frame
        cPacket *payload = check_and_cast<cPacket *>(msg);
        frame->encapsulate(payload);

        // Clean up control info
        delete request;

        // Send Ethernet frame to lower layer (MAC / network)
        send(frame, "lowerLayerOut");
        return;
    }

    // Message from lower layers (MAC / network)
    EthernetQFrame *frame = check_and_cast<EthernetQFrame *>(msg);

    // Check if frame is destined to this node or is broadcast
    if(frame->getDst() != address && frame->getDst() != std::string("FF:FF:FF:FF:FF:FF")) {
        EV_DEBUG << "Frame dropped: destination " << frame->getDst()
                 << " does not match this node (" << address << ").\n";
        delete frame;
        return;
    }

    // Decapsulate payload and forward it to upper layer
    cPacket *payload = frame->decapsulate();
    delete frame;

    EV_DEBUG << "Frame accepted: delivering payload to upper layer.\n";
    send(payload, "upperLayerOut");
}
