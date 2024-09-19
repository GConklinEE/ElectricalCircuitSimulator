#ifndef GROUNDED_VOLTAGE_SOURCE_H
#define GROUNDED_VOLTAGE_SOURCE_H

#include "CircuitComponent.h"

namespace SimulationEngine {

    class GroundedVoltageSource : public CircuitComponent {

        public:

            GroundedVoltageSource(const size_t iNodeS, const size_t iNodeD, const double dVoltage, const double dResistance); // iNodeS is assumed to be ground

            void initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void step(Matrix<double>& oSourceVector);
            void postStep(Matrix<double>& oVoltageMatrix);
            void applyConductanceMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void applySourceVectorMatrixStamp(Matrix<double>& oSourceVector);

        private:

            double m_dVoltage;
            double m_dResistance;
    };

}

#endif