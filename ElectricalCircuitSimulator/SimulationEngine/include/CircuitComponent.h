#ifndef CIRCUIT_COMPONENT_H
#define CIRCUIT_COMPONENT_H

#include "Matrix.h"

namespace SimulationEngine {

    class CircuitComponent {

        public:

            CircuitComponent(const size_t iNodeS, const size_t iNodeD, bool bIsGround);

            bool getIsGround() const {
                return m_bIsGround;
            }
            size_t getNodeS() const {
                return m_iNodeS;
            }
            size_t getNodeD() const {
                return m_iNodeD;
            }
            double getCurrent() const {
                return m_dCurrent;
            }
            virtual void initalize(Matrix& oConductanceMatrix, const double dTimeStep);
            virtual void step(Matrix& oSourceVector);
            virtual void postStep(Matrix& oVoltageMatrix);

        protected:

            virtual void applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep);
            virtual void applySourceVectorMatrixStamp(Matrix& oSourceVector);

            bool m_bIsGround;
            size_t m_iNodeS; // Source
            size_t m_iNodeD; // Destination
            double m_dComponentResistanceStamp;
            double m_dCurrent; // Current is positive if flowing from source to destination, negative if the opposite direction
    };

}

#endif