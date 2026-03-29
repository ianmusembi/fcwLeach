#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/routing/leach/MLeach.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/SignalTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/power/storage/SimpleEpEnergyStorage.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include <list>
#include <vector>
#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <fstream>
using namespace power;
namespace inet {

Define_Module(MLeach);

MLeach::MLeach() : Leach() {}

MLeach::~MLeach() {
    stop();
    delete event;
}

void MLeach::initialize(int stage) {
    Leach::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        sinkPktReceived = 0;
        
    } else if (stage == INITSTAGE_ROUTING_PROTOCOLS) {
        
    }
}

void MLeach::handleMessageWhenUp(cMessage *msg) {
    if (msg->getArrivalModule() == msg->getSenderModule()) {
        delete msg;
        return;
    }
    if(msg->isSelfMessage()){
        delete msg;
        return;
    }
    auto receivedCtrlPkt = staticPtrCast<LeachControlPkt>(check_and_cast<Packet *>(msg)->peekData<LeachControlPkt>()->dupShared());
    Packet *receivedPkt = check_and_cast<Packet *>(msg);
    auto& leachControlPkt = receivedPkt->peekAtFront<LeachControlPkt>();

    auto packetType = leachControlPkt->getPacketType();
    if (packetType == SINK) {
        sinkPktReceived++;
        // Timeout check for CHs
        if (leachState == ch && simTime() >= roundStartTime + roundDuration) {
            EV << "Node " << host->getFullName() << " CH timeout, reverting to NCH" << endl;
            setLeachState(nch);
        }
        // Only revert if it's time for a new election
        else if (msg->getKind() == SELF && leachState == ch) {
            setLeachState(nch);
        }

        double randNo = uniform(0, 1);
        threshold = generateThresholdValue(round);

        if (randNo < threshold && !wasCH) {
            weight++;
            setLeachState(ch);
            wasCH = true;
            handleSinkMessage(msg);
        }

        round++;
        int intervalLength = 1.0 / clusterHeadPercentage;
        if (fmod(round, intervalLength) == 0) {
            wasCH = false;
            nodeMemory.clear();
            nodeCHMemory.clear();
            extractedTDMASchedule.clear();
            TDMADelayCounter = 1;
        }

        // Log CH count for debugging
                // static int chCount = 0;
                // if (leachState == ch) chCount++;

                // // Move the reset to the start of a new round
                // if (fmod(round, intervalLength) == 0) {
                //     EV << "End of interval, resetting CH count from " << chCount << endl;
                //     chCount = 0;
                // }

                // EV << "Round " << round << ": Total CHs = " << chCount << " at " << simTime() << endl;

        roundStartTime = simTime();
        // event->setKind(SELF);
        // scheduleAt(simTime() + roundDuration, event);
    } else if (check_and_cast<Packet *>(msg)->getTag<PacketProtocolTag>()->getProtocol() == &Protocol::manet) {
        processMessage(msg);
    } else {
        EV_ERROR << "Message Not Supported:" << msg->getName() << simTime() << endl;
        //throw cRuntimeError("Message not supported %s", msg->getName());
    }
    refreshDisplay();
}

void MLeach::handleSinkMessage(cMessage *msg) {
    auto receivedCtrlPkt = staticPtrCast<LeachControlPkt>(check_and_cast<Packet *>(msg)->peekData<LeachControlPkt>()->dupShared());
    Packet *receivedPkt = check_and_cast<Packet *>(msg);
    auto& leachControlPkt = receivedPkt->peekAtFront<LeachControlPkt>();

    auto packetType = leachControlPkt->getPacketType();
    if (packetType == SINK) {
        auto ctrlPkt = makeShared<LeachControlPkt>();
        ctrlPkt->setPacketType(CH);
        Ipv4Address source = interface80211ptr->getProtocolData<Ipv4InterfaceData>()->getIPAddress();
        ctrlPkt->setChunkLength(b(128));
        ctrlPkt->setSrcAddress(source);

        auto packet = new Packet("LEACHControlPkt", ctrlPkt);
        auto addressReq = packet->addTag<L3AddressReq>();
        addressReq->setDestAddress(Ipv4Address(255, 255, 255, 255));
        addressReq->setSrcAddress(source);
        packet->addTag<InterfaceReq>()->setInterfaceId(interface80211ptr->getInterfaceId());
        packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::manet);
        packet->addTag<DispatchProtocolReq>()->setProtocol(&Protocol::ipv4);

        send(packet, "ipOut");
        addToEventLog(source, Ipv4Address(255, 255, 255, 255), "CTRL", "SENT");
        controlPktSent++;
        bubble("Sending new enrolment message");
    } else {
        delete msg;
    }
}


void MLeach::finish() {
    Leach::finish();
    
    EV << "Total data packets received by CH from Sinks: " << sinkPktReceived << endl;

    recordScalar("#sinkPktReceived", sinkPktReceived);
}

} // namespace inet