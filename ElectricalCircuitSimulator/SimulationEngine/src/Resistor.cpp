// This component is based on the equation: i(t) = 1/R * v(t)
//     i(t) is the component current going from + to -.
//     v(t) is the voltage potential from - to +.
//     R is the resistance.
// Matrix stamp is based on the i(t) equation for the current time step. i(t) = (Conductance Matrix Stamp) * v(t) - ((+)Node Source Vector Stamp)
// Conductance matrix stamp uses the 1/R term.
// Post step calculates i(t) for the current step.

// AcrossReferenceNode = Circuit Ground
// ComponentSimulationMatrixStamp = Component Resistance Matrix Stamp
// applyThroughVectorMatrixStamp = Component Current Vector Stamp
// Across = Voltage (V)
// Through = Current (A)

#include "Resistor.h"
#include <iostream>

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

    Resistor::Resistor(const size_t iNodeS, const size_t iNodeD, const double dResistance) :
        LinearCircuitSimComponent(iNodeS, iNodeD, false),
        m_dResistance(dResistance)
    {
        if (dResistance <= 0) {
            cout << "Resistance value must be greater than 0!" << endl;
            throw invalid_argument("Resistance value must be greater than 0!");
        }
    }

    void Resistor::LNS_initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
        m_dThrough = 0;
        applySimulationMatrixStamp(oConductanceMatrix, dTimeStep);
    }

    void Resistor::applySimulationMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
        double dResistance;

        m_dComponentSimulationMatrixStamp = 1.0 / m_dResistance;

        dResistance = oConductanceMatrix(m_iNodeS, m_iNodeS);
        oConductanceMatrix(m_iNodeS, m_iNodeS) = dResistance + m_dComponentSimulationMatrixStamp;

        dResistance = oConductanceMatrix(m_iNodeS, m_iNodeD);
        oConductanceMatrix(m_iNodeS, m_iNodeD) = dResistance - m_dComponentSimulationMatrixStamp;

        dResistance = oConductanceMatrix(m_iNodeD, m_iNodeS);
        oConductanceMatrix(m_iNodeD, m_iNodeS) = dResistance - m_dComponentSimulationMatrixStamp;

        dResistance = oConductanceMatrix(m_iNodeD, m_iNodeD);
        oConductanceMatrix(m_iNodeD, m_iNodeD) = dResistance + m_dComponentSimulationMatrixStamp;
    };

    void Resistor::LNS_postStep(Matrix<double>& oVoltageMatrix) {
        m_dThrough = (oVoltageMatrix(m_iNodeS, 0) - oVoltageMatrix(m_iNodeD, 0)) / m_dResistance;
    }

}

