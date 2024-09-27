// This component is based on the equation: i(t) = 1/R * v(t) - I
//     i(t) is the component current going from + to -.
//     v(t) is the voltage potential from - to +.
//     I is the internal source current going from - to +.
//     R is the internal source resistance.
// Matrix stamp is based on the i(t) equation for the current time step. i(t) = (Conductance Matrix Stamp) * v(t) - ((+)Node Source Vector Stamp)
// Conductance matrix stamp uses the 1/R term (internal resistance).
// Source vector stamp uses the I term (internal current source).
// Post step calculates i(t) for the current step.
// iNodeS is assumed to be ground (-), iNodeD is assumed to be (+).

// AcrossReferenceNode = Circuit Ground
// ComponentSimulationMatrixStamp = Component Resistance Matrix Stamp
// applyThroughVectorMatrixStamp = Component Current Vector Stamp
// Across = Voltage (V)
// Through = Current (A)

#include "GroundedVoltageSource.h"
#include <iostream>

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

    GroundedVoltageSource::GroundedVoltageSource(const size_t iNodeS, const size_t iNodeD, const double dVoltage, const double dResistance) :
        LinearCircuitSimComponent(0, true, iNodeS),
        m_iNodeS(iNodeS),
        m_iNodeD(iNodeD),
        m_dVoltage(dVoltage),
        m_dResistance(dResistance)
    {
        size_t iNodeList = 0;

        if (dResistance <= 0) {
            cout << "Resistance value must be greater than 0!" << endl;
            throw invalid_argument("Resistance value must be greater than 0!");
        }
        if (dVoltage <= 0) {
            cout << "Voltage value must be greater than 0!" << endl;
            throw invalid_argument("Voltage value must be greater than 0!");
        }

        iNodeList += (m_iNodeS << (0 * BITS_PER_NODE));
        iNodeList += (m_iNodeD << (1 * BITS_PER_NODE));
        setNodeList(iNodeList);
    }

    void GroundedVoltageSource::LNS_initalize(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
        m_dThrough = 0;
        applySimulationMatrixStamp(oConductanceMatrix, dTimeStep);
    }

    void GroundedVoltageSource::applySimulationMatrixStamp(Matrix<double>& oConductanceMatrix, const double dTimeStep) {
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

    void GroundedVoltageSource::applyThroughVectorMatrixStamp(Matrix<double>& oSourceVector) {
        double dCurrent;
        double dComponentCurrentStamp;

        dComponentCurrentStamp = m_dVoltage / m_dResistance;

        dCurrent = oSourceVector(m_iNodeS, 0);
        oSourceVector(m_iNodeS, 0) = dCurrent - dComponentCurrentStamp;

        dCurrent = oSourceVector(m_iNodeD, 0);
        oSourceVector(m_iNodeD, 0) = dCurrent + dComponentCurrentStamp;
    };

    void GroundedVoltageSource::LNS_step(Matrix<double>& oSourceVector) {
        applyThroughVectorMatrixStamp(oSourceVector);
    }

    void GroundedVoltageSource::LNS_postStep(Matrix<double>& oVoltageMatrix) {
        m_dThrough = (m_dVoltage - (oVoltageMatrix(m_iNodeD, 0) - oVoltageMatrix(m_iNodeS, 0))) / m_dResistance;
    }

}

