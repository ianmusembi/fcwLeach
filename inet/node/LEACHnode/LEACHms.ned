package inet.node.LEACHnode;

import inet.node.inet.AdhocHost;
import inet.routing.leach.LeachMS;
import inet.node.inet.ManetRouter;
module LEACHms extends AdhocHost {
    parameters:
        @signal[sinkStopped];
    submodules:
        LEACHms: LeachMS {
            @display("p=825,226");
        }
    connections:
        LEACHms.ipOut --> tn.in++;
        LEACHms.ipIn <-- tn.out++;
        
}