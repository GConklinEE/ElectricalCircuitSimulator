#pragma once

#include "Component.h"

namespace SimulationEngine {

    class Capacitor : public LinearCircuitSimComponent {

        public:

            Capacitor(const size_t iNodeS, const size_t iNodeD, const double m_dCapacitance);

            void LNS_initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void LNS_step(Matrix<double>& oSourceVector); // Trapezoidal integration
            void LNS_postStep(Matrix<double>& oVoltageMatrix);
            void applySimulationMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void applyThroughVectorMatrixStamp(Matrix<double>& oSourceVector);

        private:

            double m_dCapacitance;
            double m_dVoltageDelta;
    };

}