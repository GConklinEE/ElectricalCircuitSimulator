#pragma once

#include "CircuitComponent.h"

namespace SimulationEngine {

    class Resistor : public CircuitComponent {

        public:

            Resistor(const size_t iNodeS, const size_t iNodeD, const double dResistance);

            void initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void postStep(Matrix<double>& oVoltageMatrix);
            void applyConductanceMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);

        private:

            double m_dResistance;
    };

}