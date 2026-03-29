#include "LeachMS.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/routing/leach/Leach.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/SignalTag_m.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"

#include <iostream>
#include <fstream>
#include <map>
#include <omnetpp.h>
#include "inet/common/geometry/common/Coord.h"

namespace inet {

Define_Module(LeachMS);

LeachMS::LeachMS() {}

LeachMS::~LeachMS() {}

void LeachMS::initialize(int stage) {
    LeachBS::initialize(stage);
    stopSignal = registerSignal("sinkStopped");
    if (stage == INITSTAGE_ROUTING_PROTOCOLS) {
        getParentModule()->subscribe(stopSignal, this);
    }
}

void LeachMS::receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t, cObject *details) {
    Enter_Method_Silent();
    if (signalID == stopSignal) {
        EV << "MS Stop signal received at " << simTime() << endl;
        // const Coord *pos = (const Coord *)(obj);
        this->startNewRound(5.0, 6.0);
    }
}

void LeachMS::startNewRound(double currentX, double currentY){
    auto sinkPkt = makeShared<LeachSinkPkt>();
    sinkPkt->setChunkLength(b(128));
    sinkPkt->setPacketType(SINK);
    sinkPkt->setPositionX(currentX);
    sinkPkt->setPositionY(currentY);
    Ipv4Address source = interface80211ptr->getProtocolData<Ipv4InterfaceData>()->getIPAddress();
    sinkPkt->setSrcAddress(source);

    auto packet = new Packet("LEACHSinkPkt", sinkPkt);
    auto addressReq = packet->addTag<L3AddressReq>();
    addressReq->setDestAddress(Ipv4Address(255, 255, 255, 255));
    addressReq->setSrcAddress(source);
    packet->addTag<InterfaceReq>()->setInterfaceId(interface80211ptr->getInterfaceId());
    packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::manet);
    packet->addTag<DispatchProtocolReq>()->setProtocol(&Protocol::ipv4);

    send(packet, "ipOut");
    // TODO: make MS event log
    // addToEventLog(source, Ipv4Address(255, 255, 255, 255), "SINK", "SENT");
}

} // namespace inet