package inet.routing.leach;

import inet.routing.contract.IManetRouting;

simple LeachMS like IManetRouting
{
    parameters:
        @display("i=block/routing");
        string interfaceTableModule;   // The path to the InterfaceTable module
        string routingTableModule;
    gates:
        input ipIn;
        output ipOut;
}