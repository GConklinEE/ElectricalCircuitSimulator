// This component is based on the equation: i(t) = dt/2L*v(t) - (-dt/2L*v(t-1) - i(t-1))
//     i(t-x) is the component current going from + to - at t-x time steps.
//     v(t-x) is the voltage potential from - to + at t-x time steps.
//     L is the inductance.
//     dt is the simulation time step.
// Matrix stamp is based on the i(t) equation for the current time step. i(t) = (Conductance Matrix Stamp) * v(t) - ((+)Node Source Vector Stamp)
// The source vector does NOT show the currents in the system after the time step.
// Conductance matrix stamp uses the dt/2L term (internal resistance).
// Source vector stamp uses the dt/2L*v(t-1) + i(t-1) term (internal current source).
// Post step calculates i(t) for the current step.
// iNodeS is assumed to be (+), iNodeD is assumed to be (-).

#include "Inductor.h"
#include <iostream>

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

    Inductor::Inductor(const size_t iNodeS, const size_t iNodeD, const double dInductance) :
        CircuitComponent(iNodeS, iNodeD, false),
        m_dVoltageDelta(0)
    {
        if (dInductance <= 0) {
            cout << "Inductance value must be greater than 0!" << endl;
            throw invalid_argument("Inductance value must be greater than 0!");
        }
        m_dInductance = dInductance;
    }

    void Inductor::initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
        m_dCurrent = 0;
        m_dVoltageDelta = 0;
        applyConductanceMatrixStamp(oConductanceMatrix, dTimeStep);
    }

    void Inductor::applyConductanceMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
        double dResistance;

        m_dComponentResistanceStamp = dTimeStep / (2.0 * m_dInductance);

        dResistance = oConductanceMatrix(m_iNodeS, m_iNodeS);
        oConductanceMatrix(m_iNodeS, m_iNodeS) = dResistance + m_dComponentResistanceStamp;

        dResistance = oConductanceMatrix(m_iNodeS, m_iNodeD);
        oConductanceMatrix(m_iNodeS, m_iNodeD) = dResistance - m_dComponentResistanceStamp;

        dResistance = oConductanceMatrix(m_iNodeD, m_iNodeS);
        oConductanceMatrix(m_iNodeD, m_iNodeS) = dResistance - m_dComponentResistanceStamp;

        dResistance = oConductanceMatrix(m_iNodeD, m_iNodeD);
        oConductanceMatrix(m_iNodeD, m_iNodeD) = dResistance + m_dComponentResistanceStamp;
    };

    void Inductor::applySourceVectorMatrixStamp(Matrix<double>& oSourceVector) {
        double dCurrent;

        m_dCurrent = m_dComponentResistanceStamp * m_dVoltageDelta + m_dCurrent; // dt/2L*v(t-1) + i(t-1)

        dCurrent = oSourceVector(m_iNodeS, 0);
        oSourceVector(m_iNodeS, 0) = dCurrent - m_dCurrent;

        dCurrent = oSourceVector(m_iNodeD, 0);
        oSourceVector(m_iNodeD, 0) = dCurrent + m_dCurrent;
    };

    void Inductor::step(Matrix<double>& oSourceVector) { // Trapezoidal integration
        applySourceVectorMatrixStamp(oSourceVector);
    }

    void Inductor::postStep(Matrix<double>& oVoltageMatrix) {
        m_dVoltageDelta = (oVoltageMatrix(m_iNodeS, 0) - oVoltageMatrix(m_iNodeD, 0));
        m_dCurrent = m_dComponentResistanceStamp * m_dVoltageDelta + m_dCurrent; // i(t) = dt/2L*v(t) + dt/2L*v(t-1) + i(t-1), m_dCurrent = dt/2L*v(t-1) + i(t-1)
    }

}
