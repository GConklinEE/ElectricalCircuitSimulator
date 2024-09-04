// This component is based on the equation: i(t) = 1/R * v(t) - I
//     i(t) is the component current going from + to -.
//     v(t) is the voltage potential from - to +.
//     I is the internal source current going from - to +.
//     R is the internal source resistance.
// Matrix stamp is based on the i(t) equation for the current time step.
// Conductance matrix stamp uses the 1/R term (internal resistance).
// Source vector stamp uses the I term (internal current source).
// Post step calculates i(t) for the current step.
// iNodeS is assumed to be ground (-), iNodeD is assumed to be (+).

#include "GroundedVoltageSource.h"

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

	GroundedVoltageSource::GroundedVoltageSource(const int iNodeS, const int iNodeD, const double dVoltage, const double dResistance)
	: CircuitComponent(iNodeS, iNodeD, true) {
		if (dResistance <= 0) {
			cout << "Resistance value must be greater than 0!" << endl;
			throw invalid_argument("Resistance value must be greater than 0!");
		}
		if (dVoltage <= 0) {
			cout << "Voltage value must be greater than 0!" << endl;
			throw invalid_argument("Voltage value must be greater than 0!");
		}
		m_dVoltage = dVoltage;
		m_dResistance = dResistance;
	}

	void GroundedVoltageSource::initalize(Matrix& oConductanceMatrix, const double dTimeStep) {
		applyConductanceMatrixStamp(oConductanceMatrix, dTimeStep);
	}

	void GroundedVoltageSource::applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) {
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

	void GroundedVoltageSource::step(Matrix& oSourceVector) {
		double dCurrent;
		double dComponentCurrentStamp;

		dComponentCurrentStamp = m_dVoltage / m_dResistance;

		dCurrent = oSourceVector.getValue(m_iNodeS, 0);
		oSourceVector.setValue(m_iNodeS, 0, dCurrent - dComponentCurrentStamp);

		dCurrent = oSourceVector.getValue(m_iNodeD, 0);
		oSourceVector.setValue(m_iNodeD, 0, dCurrent + dComponentCurrentStamp);
	}

	void GroundedVoltageSource::postStep(Matrix& oVoltageMatrix) {
		m_dCurrent = (m_dVoltage - (oVoltageMatrix.getValue(m_iNodeD, 0) - oVoltageMatrix.getValue(m_iNodeS, 0))) / m_dResistance;
	}

}

