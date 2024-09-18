#ifndef RESISTOR_H
#define RESISTOR_H

#include "CircuitComponent.h"

namespace SimulationEngine {

    class Resistor : public CircuitComponent {

        public:

            Resistor(const int iNodeS, const int iNodeD, const double dResistance);

            void initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void postStep(Matrix<double>& oVoltageMatrix);
            void applyConductanceMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);

        private:

            double m_dResistance;
    };

}

#endif