#ifndef __INET_SINKMOBILITY_H
#define __INET_SINKMOBILITY_H

#include "inet/mobility/base/MovingMobilityBase.h"

namespace inet {

/**
 * @brief Rectangle sink movement model. See NED file for more info.
 *
 * @ingroup mobility
 */
class INET_API SinkMobility : public MovingMobilityBase
{
  protected:
    // configuration
    double speed; ///< speed of the host
    double startedWait;
    double sojournTime;
    double currentSpeed;
    simsignal_t stopSignal;

    // state
    double d; ///< distance from (x1,y1), measured clockwise on the perimeter
    double corner1, corner2, corner3, corner4;
    double sojourn1, sojourn2, sojourn3, sojourn4;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    /** @brief Initializes mobility model parameters.
     *
     * If the host is not stationary it calculates a random position on the rectangle.
     */
    virtual void initialize(int stage) override;

    /** @brief Initializes the position according to the mobility model. */
    virtual void setInitialPosition() override;

    /** @brief Move the host */
    virtual void move() override;

  public:
    virtual double getMaxSpeed() const override { return speed; }
    SinkMobility();
};

} // namespace inet

#endif