#pragma once

#include "Component.h"

namespace SimulationEngine {

    class GroundedVoltageSource : public LinearCircuitSimComponent {

        public:

            GroundedVoltageSource(const size_t iNodeS, const size_t iNodeD, const double dVoltage, const double dResistance); // iNodeS is assumed to be ground

            void LNS_initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void LNS_step(Matrix<double>& oSourceVector);
            void LNS_postStep(Matrix<double>& oVoltageMatrix);
            void applySimulationMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void applyThroughVectorMatrixStamp(Matrix<double>& oSourceVector);

        private:

            size_t m_iNodeS;
            size_t m_iNodeD;
            double m_dVoltage;
            double m_dResistance;
    };

}