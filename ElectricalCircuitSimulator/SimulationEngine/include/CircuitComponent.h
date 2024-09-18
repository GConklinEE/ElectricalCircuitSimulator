#ifndef CIRCUIT_COMPONENT_H
#define CIRCUIT_COMPONENT_H

#include "Matrix.h"

namespace SimulationEngine {

    class CircuitComponent {

        public:

            CircuitComponent(const int iNodeS, const int iNodeD, bool bIsGround);

            bool getIsGround() const {
                return m_bIsGround;
            }
            int getNodeS() const {
                return m_iNodeS;
            }
            int getNodeD() const {
                return m_iNodeD;
            }
            double getCurrent() const {
                return m_dCurrent;
            }
            virtual void initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            virtual void step(Matrix<double>& oSourceVector);
            virtual void postStep(Matrix<double>& oVoltageMatrix);

        protected:

            virtual void applyConductanceMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep);
            virtual void applySourceVectorMatrixStamp(Matrix<double>& oSourceVector);

            bool m_bIsGround;
            int m_iNodeS; // Source
            int m_iNodeD; // Destination
            double m_dComponentResistanceStamp;
            double m_dCurrent; // Current is positive if flowing from source to destination, negative if the opposite direction
    };

}

#endif