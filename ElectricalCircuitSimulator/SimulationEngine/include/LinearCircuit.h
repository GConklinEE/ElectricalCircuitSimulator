#ifndef LINEARCIRCUIT_H
#define LINEARCIRCUIT_H

#include "CircuitComponent.h"
#include <vector>

using std::vector;
using std::unique_ptr;

namespace SimulationEngine {

    class LinearCircuit final {

        public:

            LinearCircuit(const int iNumComponents);
            LinearCircuit(const LinearCircuit&) = delete;
            LinearCircuit(LinearCircuit&&) = delete;

            ~LinearCircuit();

            LinearCircuit& operator=(const LinearCircuit&) = delete;
            LinearCircuit& operator=(LinearCircuit&&) = delete;

            int addComponent(unique_ptr<CircuitComponent> pCircuitComponent);
            void setStopTime(const double dStopTime);
            void setTimeStep(const double dTimeStep);
            double getTime() const;
            double getVoltage(const int iNode) const;
            double getCurrent(const int iComponentIndex) const;
            void initalize();
            bool step();

        private:

            unique_ptr<CircuitComponent>* m_pCircuitComponents;
            Matrix* m_pConductanceMatrix;
            Matrix* m_pSourceVector;
            Matrix* m_pVoltageVector;
            PLU_Factorization* m_pPLU_Factorization;
            vector<int> m_oNodeList;
            int m_iMaxComponentCount;
            int m_iComponentCount;
            int m_iMaxNode;
            int m_iGroundNode;
            double m_dStopTime;
            double m_dTimeStep;
            double m_dTime;
            bool m_bHasGround;
            bool m_bInitSim;
            bool m_bRunSim;
    };

}

#endif
