#ifndef __INET_LEACHMS_H__
#define __INET_LEACHMS_H__

#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "inet/routing/leach/LeachBS.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/routing/leach/LeachPkts_m.h"
#include <map>

namespace inet {

class INET_API LeachMS : public LeachBS, public cIListener {
  private:
    simsignal_t stopSignal;
    
  public:
    LeachMS();
    virtual ~LeachMS();
  
  protected:
    virtual void initialize(int stage) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details) override {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l, cObject *details) override {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l, cObject *details) override {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details) override {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s, cObject *details) override {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t, cObject *details) override {}
    void startNewRound(double currentX, double currentY, double sojourn);
};

} // namespace inet

#endif // __INET_LEACHMS_H__