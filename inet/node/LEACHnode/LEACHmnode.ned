
package inet.node.LEACHnode;

import inet.node.inet.AdhocHost;
import inet.routing.leach.MLeach;
import inet.node.inet.ManetRouter;
module LEACHmnode extends AdhocHost
{
    @display("bgb=1666.404,1029.6481");
    submodules:
        LEACHmnode: MLeach {
            @display("p=825,226");
        }
    connections:
        LEACHmnode.ipOut --> tn.in++;
        LEACHmnode.ipIn <-- tn.out++;

}