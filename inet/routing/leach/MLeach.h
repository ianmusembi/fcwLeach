#ifndef __INET_MLEACH_H__
#define __INET_MLEACH_H__

#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/contract/IRoutingTable.h"
#include "inet/routing/leach/Leach.h"
#include "inet/routing/leach/LeachPkts_m.h"
#include "inet/power/storage/SimpleEpEnergyStorage.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/common/geometry/common/Coord.h"

namespace inet {

/**
 * @brief Adds mobility to LEACH protocol for OMNeT++ and INET 4.5
 *
 * This implementation provides:
 * - Cluster Head election based on adaptive threshold
 * - Node-to-CH association and TDMA scheduling
 * - Data transmission to CH and from CH to base station
 * - Energy monitoring and state management
 */
class INET_API MLeach : public Leach {
  private:
    int sinkPktReceived = 0;

  public:
    MLeach();
    virtual ~MLeach();

  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;

    void handleSinkMessage(cMessage *msg);

    virtual void finish() override;

};

} // namespace inet

#endif // __INET_LEACH_H__
