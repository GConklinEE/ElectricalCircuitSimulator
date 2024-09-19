#include "CircuitComponent.h"
#include <iostream>

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

    // If the component has a ground, iNodeS is assumed to be that ground
    CircuitComponent::CircuitComponent(const size_t iNodeS, const size_t iNodeD, bool bIsGround) :
        m_bIsGround(bIsGround),
        m_dCurrent(0.0),
        m_dComponentResistanceStamp(0.0)
    {
        if (iNodeS == iNodeD) {
            cout << "Node values must not be the same!" << endl;
            throw invalid_argument("Node values must not be the same!");
        }
        m_iNodeS = iNodeS;
        m_iNodeD = iNodeD;
    }

    void CircuitComponent::initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
        ;
    }

    void CircuitComponent::step(Matrix<double>& oSourceVector) {
        ;
    }

    void CircuitComponent::postStep(Matrix<double>& oVoltageMatrix) {
        ;
    }

    void CircuitComponent::applyConductanceMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
        ;
    }

    void CircuitComponent::applySourceVectorMatrixStamp(Matrix<double>& oSourceVector) {
        ;
    }

}