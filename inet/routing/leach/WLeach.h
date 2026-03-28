#ifndef __INET_WLEACH_H
#define __INET_WLEACH_H

#include "inet/common/INETDefs.h"
#include "inet/routing/leach/Leach.h"

namespace inet {

    class INET_API WLeach : public Leach
    {
        protected:
            // weight coefficients
            double alpha = 0.5;
            double beta = 0.5;
            double gamma = 0.5;

            // weighted leach states
            double currentWeight = 0.0;
            int neighborCount = 0;
            double densityNorm = 0.0;
            double energyNorm = 0.0;
            double thresholdFloor = 0.01;
            int neighborSaturationK = 10; // local density cap

        protected:
            virtual void initialize(int stage) override;
            virtual void handleMessageWhenUp(cMessage *msg) override;
            virtual void finish() override;

            // helpers for W-LEACH
            virtual double computeEnergyNorm();
            virtual int computeNeighbourCount();
            virtual double computeDensityNorm();
            virtual double computeWeight();
            virtual double computeWeightedThreshold(int round);

            // optional debug/stat helpers
            virtual void recordWeightSnapshot();

    };
} // namespace inet

#endif