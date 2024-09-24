// This component is based on the equation: i(t) = 2C/dt*v(t) - (2C/dt*v(t-1) + i(t-1))
//     i(t-x) is the component current going from + to - at t-x time steps.
//     v(t-x) is the voltage potential from - to + at t-x time steps.
//     C is the capacitance.
//     dt is the simulation time step.
// Matrix stamp is based on the i(t) equation for the current time step. i(t) = (Conductance Matrix Stamp) * v(t) - ((+)Node Source Vector Stamp)
// The source vector does NOT show the currents in the system after the time step.
// Conductance matrix stamp uses the 2C/dt term (internal resistance).
// Source vector stamp uses the 2C/dt*v(t-1) + i(t-1) term (internal current source).
// Post step calculates i(t) for the current step.
// iNodeS is assumed to be (+), iNodeD is assumed to be (-).

// AcrossReferenceNode = Circuit Ground
// ComponentSimulationMatrixStamp = Component Resistance Matrix Stamp
// applyThroughVectorMatrixStamp = Component Current Vector Stamp
// Across = Voltage (V)
// Through = Current (A)

#include "Capacitor.h"
#include <iostream>

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

    Capacitor::Capacitor(const size_t iNodeS, const size_t iNodeD, const double dCapacitance) :
        LinearCircuitSimComponent(iNodeS, iNodeD, false),
        m_dCapacitance(dCapacitance),
        m_dVoltageDelta(0)
    {
        if (dCapacitance <= 0) {
            cout << "Capacitance value must be greater than 0!" << endl;
            throw invalid_argument("Capacitance value must be greater than 0!");
        }
    }

    void Capacitor::LNS_initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
        m_dThrough = 0;
        m_dVoltageDelta = 0;
        applySimulationMatrixStamp(oConductanceMatrix, dTimeStep);
    }

    void Capacitor::applySimulationMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
        double dResistance;

        m_dComponentSimulationMatrixStamp = (2.0 * m_dCapacitance) / dTimeStep;

        dResistance = oConductanceMatrix(m_iNodeS, m_iNodeS);
        oConductanceMatrix(m_iNodeS, m_iNodeS) = dResistance + m_dComponentSimulationMatrixStamp;

        dResistance = oConductanceMatrix(m_iNodeS, m_iNodeD);
        oConductanceMatrix(m_iNodeS, m_iNodeD) = dResistance - m_dComponentSimulationMatrixStamp;

        dResistance = oConductanceMatrix(m_iNodeD, m_iNodeS);
        oConductanceMatrix(m_iNodeD, m_iNodeS) = dResistance - m_dComponentSimulationMatrixStamp;

        dResistance = oConductanceMatrix(m_iNodeD, m_iNodeD);
        oConductanceMatrix(m_iNodeD, m_iNodeD) = dResistance + m_dComponentSimulationMatrixStamp;
    };

    void Capacitor::applyThroughVectorMatrixStamp(Matrix<double>& oSourceVector) {
        double dCurrent;

        m_dThrough = m_dComponentSimulationMatrixStamp * (m_dVoltageDelta) + m_dThrough; // 2C/dt*v(t-1) + i(t-1)

        dCurrent = oSourceVector(m_iNodeS, 0);
        oSourceVector(m_iNodeS, 0) = dCurrent + m_dThrough;

        dCurrent = oSourceVector(m_iNodeD, 0);
        oSourceVector(m_iNodeD, 0) = dCurrent - m_dThrough;
    };

    void Capacitor::LNS_step(Matrix<double>& oSourceVector) { // Trapezoidal integration
        applyThroughVectorMatrixStamp(oSourceVector);
    }

    void Capacitor::LNS_postStep(Matrix<double>& oVoltageMatrix) {
        m_dVoltageDelta = (oVoltageMatrix(m_iNodeS, 0) - oVoltageMatrix(m_iNodeD, 0));
        m_dThrough = m_dComponentSimulationMatrixStamp * (m_dVoltageDelta) -m_dThrough; // i(t) = 2C/dt*v(t) - 2C/dt*v(t-1) - i(t-1), m_dThrough = 2C/dt*v(t-1) + i(t-1)
    }

}