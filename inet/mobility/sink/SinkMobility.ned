package inet.mobility.sink;

import inet.mobility.base.MovingMobilityBase;

//
// Moves the mobile around a rectangle, stopping at sojourn positions.
//
simple SinkMobility extends MovingMobilityBase
{
    parameters:
        double startPos; // in range [0.0,4.0): topleft=0, topright=1, bottomright=2, bottomleft=3
        double speed @unit(mps); // speed of the host
        double sojournTime; // how long to stop at sojourn position
        @class(SinkMobility);
}