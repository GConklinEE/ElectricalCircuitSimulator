#pragma once

#include "Component.h"

namespace SimulationEngine {

    class Inductor : public LinearCircuitSimComponent {

        public:

            Inductor(const size_t iNodeS, const size_t iNodeD, const double m_dInductance);

            void LNS_initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void LNS_step(Matrix<double>& oSourceVector); // Trapezoidal integration
            void LNS_postStep(Matrix<double>& oVoltageMatrix);
            void applySimulationMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void applyThroughVectorMatrixStamp(Matrix<double>& oSourceVector);

        private:

            size_t m_iNodeS;
            size_t m_iNodeD;
            double m_dInductance;
            double m_dVoltageDelta;
    };

}