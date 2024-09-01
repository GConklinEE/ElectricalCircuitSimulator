#include "GroundedVoltageSource.h"

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

	GroundedVoltageSource::GroundedVoltageSource(const int iNodeS, const int iNodeD, const double dVoltage, const double dResistance)
	: CircuitComponent(iNodeS, iNodeD, true) { // iNodeD is assumed to be ground
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

	void GroundedVoltageSource::applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) const {
		double dResistance;
		double dComponentResistance = 1.0 / m_dResistance;

		dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeS);
		oConductanceMatrix.setValue(m_iNodeS, m_iNodeS, dResistance + dComponentResistance);

		dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeD);
		oConductanceMatrix.setValue(m_iNodeS, m_iNodeD, dResistance - dComponentResistance);

		dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeS);
		oConductanceMatrix.setValue(m_iNodeD, m_iNodeS, dResistance - dComponentResistance);

		dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeD);
		oConductanceMatrix.setValue(m_iNodeD, m_iNodeD, dResistance + dComponentResistance);
	};

	void GroundedVoltageSource::step(Matrix& oSourceVector, const double dTimeStep) {
		double dCurrent;
		double dComponentCurrent;

		dComponentCurrent = m_dVoltage / m_dResistance;
		dCurrent = oSourceVector.getValue(m_iNodeD, 0);
		oSourceVector.setValue(m_iNodeD, 0, dCurrent + dComponentCurrent);
		dCurrent = oSourceVector.getValue(m_iNodeS, 0);
		oSourceVector.setValue(m_iNodeS, 0, dCurrent - dComponentCurrent);
	}

	void GroundedVoltageSource::postStep(Matrix& oVoltageMatrix, const double dTimeStep) {
		m_dCurrent = (oVoltageMatrix.getValue(m_iNodeS, 0) - oVoltageMatrix.getValue(m_iNodeD, 0)) / m_dResistance;
	}

}

