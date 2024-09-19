#ifndef RESISTOR_H
#define RESISTOR_H

#include "CircuitComponent.h"

namespace SimulationEngine {

    class Resistor : public CircuitComponent {

        public:

            Resistor(const size_t iNodeS, const size_t iNodeD, const double dResistance);

            void initalize(Matrix& oConductanceMatrix, const double dTimeStep);
            void postStep(Matrix& oVoltageMatrix);
            void applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep);

        private:

            double m_dResistance;
    };

}

#endif