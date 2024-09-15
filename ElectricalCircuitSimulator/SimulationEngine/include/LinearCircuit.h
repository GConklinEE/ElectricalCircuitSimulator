#ifndef LINEARCIRCUIT_H
#define LINEARCIRCUIT_H

#include "CircuitComponent.h"
#include <vector>

namespace SimulationEngine {

    class LinearCircuit final {

        public:

            LinearCircuit(const size_t iNumComponents);

            size_t addComponent(std::unique_ptr<CircuitComponent> pCircuitComponent);
            void setStopTime(const double dStopTime);
            void setTimeStep(const double dTimeStep);
            double getTime() const;
            double getVoltage(const size_t iNode) const;
            double getCurrent(const size_t iComponentIndex) const;
            void initalize();
            bool step();

        private:

            void solver();

            std::unique_ptr<std::unique_ptr<CircuitComponent>[]> m_pCircuitComponents;
            std::unique_ptr<Matrix> m_pConductanceMatrix;
            std::unique_ptr<Matrix> m_pSourceVector;
            std::unique_ptr<Matrix> m_pVoltageVector;
            std::unique_ptr<PLU_Factorization> m_pPLU;
            std::vector<size_t> m_oNodeList;
            size_t m_iMaxComponentCount;
            size_t m_iComponentCount;
            size_t m_iMaxNode;
            size_t m_iGroundNode;
            double m_dStopTime;
            double m_dTimeStep;
            double m_dTime;
            bool m_bHasGround;
            bool m_bInitSim;
            bool m_bRunSim;
    };

}

#endif
