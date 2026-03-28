package inet.node.LEACHnode;

import inet.node.inet.AdhocHost;
import inet.routing.leach.WLeach;

module WLEACHnode extends AdhocHost
{
    parameters:
        double alpha = default(0.5);
        double beta = default(0.5);
        double thresholdFloor = default(0.01);
        int neighborSaturationK = default(10);
        double commRange @unit(m) = default(100m);

        @display("bgb=1666.404,1029.6481");

    submodules:
        routing: WLeach {
            parameters:
                alpha = parent.alpha;
                beta = parent.beta;
                thresholdFloor = parent.thresholdFloor;
                neighborSaturationK = parent.neighborSaturationK;
                commRange = parent.commRange;
            @display("p=825,226");
        }

    connections:
        routing.ipOut --> tn.in++;
        routing.ipIn <-- tn.out++;
}