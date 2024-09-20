#pragma once

#include "CircuitComponent.h"

namespace SimulationEngine {

    class Capacitor : public CircuitComponent {

        public:

            Capacitor(const size_t iNodeS, const size_t iNodeD, const double m_dCapacitance);

            void initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void step(Matrix<double>& oSourceVector); // Trapezoidal integration
            void postStep(Matrix<double>& oVoltageMatrix);
            void applyConductanceMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void applySourceVectorMatrixStamp(Matrix<double>& oSourceVector);

        private:

            double m_dCapacitance;
            double m_dVoltageDelta;
    };

}