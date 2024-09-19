#ifndef LINEARCIRCUIT_H
#define LINEARCIRCUIT_H

#include "CircuitComponent.h"
#include "PLU_Factorization.h"
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
    size_t m_iMaxComponentCount;
    std::unique_ptr<std::unique_ptr<CircuitComponent>[]> m_pCircuitComponents;
    bool m_bHasGround;
    int m_iComponentCount;
    size_t m_iMaxNode;
    size_t m_iGroundNode;
    double m_dStopTime;
    double m_dTimeStep;
    double m_dTime;
    bool m_bInitSim;
    bool m_bRunSim;
    Matrix<double> m_pConductanceMatrix;
    Matrix<double> m_pSourceVector;
    Matrix<double> m_pVoltageVector;
    PLU_Factorization<double> m_pPLU_Factorization;
    std::vector<size_t> m_oNodeList;
  };
}

#endif