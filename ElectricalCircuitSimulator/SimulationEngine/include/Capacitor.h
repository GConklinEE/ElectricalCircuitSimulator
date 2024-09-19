#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "CircuitComponent.h"

namespace SimulationEngine {

    class Capacitor : public CircuitComponent {

        public:

            Capacitor(const size_t iNodeS, const size_t iNodeD, const double m_dCapacitance);

            void initalize(Matrix& oConductanceMatrix, const double dTimeStep);
            void step(Matrix& oSourceVector); // Trapezoidal integration
            void postStep(Matrix& oVoltageMatrix);
            void applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep);
            void applySourceVectorMatrixStamp(Matrix& oSourceVector);

        private:

            double m_dCapacitance;
            double m_dVoltageDelta;
    };

}

#endif