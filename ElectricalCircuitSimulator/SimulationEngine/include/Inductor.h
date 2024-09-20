#pragma once

#include "CircuitComponent.h"

namespace SimulationEngine {

    class Inductor : public CircuitComponent {

        public:

            Inductor(const size_t iNodeS, const size_t iNodeD, const double m_dInductance);

            void initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void step(Matrix<double>& oSourceVector); // Trapezoidal integration
            void postStep(Matrix<double>& oVoltageMatrix);
            void applyConductanceMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            void applySourceVectorMatrixStamp(Matrix<double>& oSourceVector);

        private:

            double m_dInductance;
            double m_dVoltageDelta;
    };

}