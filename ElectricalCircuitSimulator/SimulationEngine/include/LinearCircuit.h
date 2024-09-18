#ifndef LINEARCIRCUIT_H
#define LINEARCIRCUIT_H

#include "CircuitComponent.h"
#include "PLU_Factorization.h"
#include <vector>

namespace SimulationEngine
{
  class LinearCircuit final
  {
  public:
    LinearCircuit(const int iNumComponents);
    LinearCircuit(const LinearCircuit&) = delete;
    LinearCircuit(LinearCircuit&&) = delete;

    LinearCircuit& operator=(const LinearCircuit&) = delete;
    LinearCircuit& operator=(LinearCircuit&&) = delete;

    int addComponent(std::unique_ptr<CircuitComponent> pCircuitComponent);
    void setStopTime(const double dStopTime);
    void setTimeStep(const double dTimeStep);
    double getTime() const;
    double getVoltage(const int iNode) const;
    double getCurrent(const int iComponentIndex) const;
    void initalize();
    bool step();

  private:
    int m_iMaxComponentCount;
    std::unique_ptr<std::unique_ptr<CircuitComponent>[]> m_pCircuitComponents;
    bool m_bHasGround;
    int m_iComponentCount;
    int m_iMaxNode;
    int m_iGroundNode;
    double m_dStopTime;
    double m_dTimeStep;
    double m_dTime;
    bool m_bInitSim;
    bool m_bRunSim;
    Matrix<double> m_pConductanceMatrix;
    Matrix<double> m_pSourceVector;
    Matrix<double> m_pVoltageVector;
    PLU_Factorization<double> m_pPLU_Factorization;
    std::vector<int> m_oNodeList;
  };
}

#endif