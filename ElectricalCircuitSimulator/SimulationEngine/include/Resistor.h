#pragma once

#include "Component.h"

namespace SimulationEngine {

    class Resistor : public LinearCircuitSimComponent {

        public:

            Resistor(const size_t iNodeS, const size_t iNodeD, const double dResistance);

            void LNS_initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void LNS_postStep(Matrix<double>& oVoltageMatrix);
            void applySimulationMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);

        private:

            size_t m_iNodeS;
            size_t m_iNodeD;
            double m_dResistance;
    };

}