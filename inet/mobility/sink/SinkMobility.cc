#include "inet/mobility/sink/SinkMobility.h"

#include "inet/common/INETMath.h"

namespace inet {

Define_Module(SinkMobility);

SinkMobility::SinkMobility()
{
    speed = 0;
    currentSpeed = 0;
    d = 0;
    corner1 = corner2 = corner3 = corner4 = 0;
    sojournTime = 2;
    startedWait = 0;
}

void SinkMobility::initialize(int stage)
{
    MovingMobilityBase::initialize(stage);

    EV_TRACE << "initializing RectangleMobility stage " << stage << endl;
    if (stage == INITSTAGE_LOCAL) {
        speed = par("speed");
        currentSpeed = speed;
        stationary = (speed == 0);

        // calculate helper variables
        double dx = constraintAreaMax.x - constraintAreaMin.x;
        double dy = constraintAreaMax.y - constraintAreaMin.y;
        corner1 = dx;
        corner2 = corner1 + dy;
        corner3 = corner2 + dx;
        corner4 = corner3 + dy;

        // determine start position
        double startPos = par("startPos");
        startPos = fmod(startPos, 4);
        if (startPos < 1)
            d = startPos * dx; // top side
        else if (startPos < 2)
            d = corner1 + (startPos - 1) * dy; // right side
        else if (startPos < 3)
            d = corner2 + (startPos - 2) * dx; // bottom side
        else
            d = corner3 + (startPos - 3) * dy; // left side
        WATCH(d);
    }
}

void SinkMobility::setInitialPosition()
{
    MovingMobilityBase::setInitialPosition();
    move();
}

void SinkMobility::move()
{
    simtime_t now = simTime();
    double wait = (now).dbl() - startedWait;
    if(currentSpeed == 0 && wait >= sojournTime){
        currentSpeed = speed;
    }
    if(currentSpeed == 0){
        return;
    }
    double elapsedTime = (now - lastUpdate).dbl();
    double newD = d + (currentSpeed * elapsedTime); //want to save d, if we pass corner, stop there

    // while (newD < 0)
    //     newD += corner4;

    // while (newD >= corner4)
    //     newD -= corner4;
    if(d == newD){
        d = newD;
    }
    else if(d < corner1 && corner1 <= newD){
        currentSpeed = 0;
        startedWait = (now).dbl();
        d = corner1;
    }
    else if(d < corner2 && corner2 <= newD){
        currentSpeed = 0;
        startedWait = (now).dbl();
        d = corner2;
    } 
    else if(d < corner3 && corner3 <= newD){
        currentSpeed = 0;
        startedWait = (now).dbl();
        d = corner3;
    }
    else if(d < corner4 && corner4 <= newD){
        currentSpeed = 0;
        startedWait = (now).dbl();
        d = 0;
    }
    else{
        d = newD;
    }

    if (d < corner1) {
        // top side
        lastPosition.x = constraintAreaMin.x + d;
        lastPosition.y = constraintAreaMin.y;
        lastVelocity = Coord(currentSpeed, 0, 0);
    }
    else if (d < corner2) {
        // right side
        lastPosition.x = constraintAreaMax.x;
        lastPosition.y = constraintAreaMin.y + d - corner1;
        lastVelocity = Coord(0, currentSpeed, 0);
    }
    else if (d < corner3) {
        // bottom side
        lastPosition.x = constraintAreaMax.x - d + corner2;
        lastPosition.y = constraintAreaMax.y;
        lastVelocity = Coord(-currentSpeed, 0, 0);
    }
    else {
        // left side
        lastPosition.x = constraintAreaMin.x;
        lastPosition.y = constraintAreaMax.y - d + corner3;
        lastVelocity = Coord(0, -currentSpeed, 0);
    }
}

} // namespace inet

