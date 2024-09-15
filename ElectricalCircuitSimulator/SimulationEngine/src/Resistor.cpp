// This component is based on the equation: i(t) = 1/R * v(t)
//     i(t) is the component current going from + to -.
//     v(t) is the voltage potential from - to +.
//     R is the resistance.
// Matrix stamp is based on the i(t) equation for the current time step. i(t) = (Conductance Matrix Stamp) * v(t) - ((+)Node Source Vector Stamp)
// Conductance matrix stamp uses the 1/R term.
// Post step calculates i(t) for the current step.

#include "Resistor.h"

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

    Resistor::Resistor(const int iNodeS, const int iNodeD, const double dResistance)
    : CircuitComponent(iNodeS, iNodeD, false) {
        if (dResistance <= 0) {
            cout << "Resistance value must be greater than 0!" << endl;
            throw invalid_argument("Resistance value must be greater than 0!");
        }
        m_dResistance = dResistance;
    }

    void Resistor::initalize(Matrix& oConductanceMatrix, const double dTimeStep) {
        m_dCurrent = 0;
        applyConductanceMatrixStamp(oConductanceMatrix, dTimeStep);
    }

    void Resistor::applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) {
        double dResistance;

        m_dComponentResistanceStamp = 1.0 / m_dResistance;

        dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeS);
        oConductanceMatrix.setValue(m_iNodeS, m_iNodeS, dResistance + m_dComponentResistanceStamp);

        dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeD);
        oConductanceMatrix.setValue(m_iNodeS, m_iNodeD, dResistance - m_dComponentResistanceStamp);

        dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeS);
        oConductanceMatrix.setValue(m_iNodeD, m_iNodeS, dResistance - m_dComponentResistanceStamp);

        dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeD);
        oConductanceMatrix.setValue(m_iNodeD, m_iNodeD, dResistance + m_dComponentResistanceStamp);
    };

    void Resistor::postStep(Matrix& oVoltageMatrix) {
        m_dCurrent = (oVoltageMatrix.getValue(m_iNodeS, 0) - oVoltageMatrix.getValue(m_iNodeD, 0)) / m_dResistance;
    }

}

