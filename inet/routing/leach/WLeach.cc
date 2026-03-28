#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/InitStages.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/routing/leach/WLeach.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/SignalTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/power/storage/SimpleEpEnergyStorage.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "omnetpp/cexception.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cmodule.h"
#include <list>
#include <vector>
#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <fstream>
using namespace power;
namespace inet {

Define_Module(WLeach);


void WLeach::initialize(int stage) {
    Leach::initialize(stage);
    
    if (stage == INITSTAGE_LOCAL){
        alpha = par("alpha").doubleValue();
        beta = par("beta").doubleValue();
        thresholdFloor = par("thresholdFloor").doubleValue();
        neighborSaturationK = par("neighborSaturationK").intValue();
        currentWeight = 0.0;
    }
}


void WLeach::handleMessageWhenUp(cMessage *msg) {
    if (msg->isSelfMessage()) {
        // Timeout check for CHs
                if (leachState == ch && simTime() >= roundStartTime + roundDuration) {
                    EV << "Node " << host->getFullName() << " CH timeout, reverting to NCH" << endl;
                    setLeachState(nch);
                }
                // Only revert if it's time for a new election
                else if (msg->getKind() == SELF && leachState == ch) {
                    setLeachState(nch);
                }
                
        // W-LEACH CH SELECTION LOGIC

        double randNo = uniform(0, 1);
        threshold = generateThresholdValue(round); // TODO: CHANGE TO computeWeightedThreshold() function

        if (randNo < threshold && !wasCH) {
            // currentWeight = computeWeight()
            weight = currentWeight;
            setLeachState(ch);
            wasCH = true;
            totalChCount++;
            handleSelfMessage(msg);
        }
        // end of W-LEACH


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
                static int chCount = 0;
                if (leachState == ch) chCount++;

                // Move the reset to the start of a new round
                if (fmod(round, intervalLength) == 0) {
                    EV << "End of interval, resetting CH count from " << chCount << endl;
                    chCount = 0;
                }

                EV << "Round " << round << ": Total CHs = " << chCount << " at " << simTime() << endl;

        roundStartTime = simTime();
        event->setKind(SELF);
        scheduleAt(simTime() + roundDuration, event);
    } else if (check_and_cast<Packet *>(msg)->getTag<PacketProtocolTag>()->getProtocol() == &Protocol::manet) {
        processMessage(msg);
    } else {
        EV_ERROR << "Message Not Supported:" << msg->getName() << simTime() << endl;
        //throw cRuntimeError("Message not supported %s", msg->getName());
    }
    refreshDisplay();
}


double WLeach::computeEnergyNorm(){
    try{
        auto *energyStorage = check_and_cast<SimpleEpEnergyStorage *>(host->getSubmodule("energyStorage"));
        double residual = energyStorage->getResidualEnergyCapacity().get();
        double initial = energyStorage->getNominalEnergyCapacity().get();

        if (initial <= 0) {
            return 0.0;
        }

        return std::max(0.0, std::min(1.0, residual/initial));
    } catch (const cRuntimeError){
        return 0.0;
    }
}

double WLeach::computeSimpleNeighborCount(){
    // Simple first-pass implementation:
    // use already-known node positions and communication radius assumptions later.
    // For now, reuse nodeMemory size as a local approximation once CH messages are heard.
    // node memory stores nodes that this node has heard from
    return static_cast<int>(nodeMemory.size());
}

double WLeach::computeNeighborCount(){
    int count = 0;

    auto *myMobility = check_and_cast<IMobility *>(host->getSubmodule("mobility"));
    Coord myPos = myMobility->getCurrentPosition();

    double commRange = par("commRange").doubleValue();


    cModule *network = host->getParentMmodule();
    if (!network){
        return 0.0;
    }

    int totalNodes = network->par("numNodes").intValue();

    for (int i = 0; i < totalNodes; i++){
        cModule *otherHost = network->getSubmodule("host", i);

        if (!otherHost){
            continue;
        }

        if (otherHost == host) { // skkp self
            continue;
        }

        //get other node position
        auto *myMobility = dynamic_cast<IMobility *>(otherHost->getSubmodule("mobility"));
        if (!otherMobility){
            continue;
        }

        // check if other node is alive
        auto* energyStorage = dynamic_cast<inet::power::SimpleEpEnergyStorage *>(otherHost->getSubmodule("energyStorage"));
        if (!energyStorage){
            continue;
        }

        double residual energy = energyStorage.getResidualEnergyCapacity.get();
        if (residualEnergy <= 0){
            continue;
        }

        Coord otherPos = otherMobility->getCurrentPosition();

        if (myPos.distance(otherPos) <= commRange){
            count++;
        }
    }
    return count;
}

double WLeach::computeDensityNorm(){
    neighborCount = computeNeighbourCount();
    if (neighborSaturationK <= 0){
        return 0.0;
    }

    double density = static_cast<double>(neighborCount) / neighborSaturationK;
    return std::max(0.0, std::min(1.0, density));
}

double WLeach::computeWeight(){
    energyNorm = computeEnergyNorm();
    densityNorm = computeDensityNorm();

    double weight = (alpha * energyNorm) + (beta * densityNorm);
    return std::max(0.0, std::min(1.0, weight))
}

double WLeach::computeWeightedThreshold(int round){
    double tLeach = generateThresholdValue(round);
    double w = computeWeight();

    // Keep decentralized LEACH threshold, but bias it by weight
    double tFinal = std::max(thresholdFloor, (tLeach * w));
    r
    
    return std::max(0.0, std::min(1.0, tFinal));
}

void WLeach::finish()
{
    addToNodeWeightList(); // maybe abstract this into a logging function and call that function here
    Leach::finish();
}


} // namespace inet
