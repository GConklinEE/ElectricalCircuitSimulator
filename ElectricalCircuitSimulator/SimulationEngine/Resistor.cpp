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
		applyConductanceMatrixStamp(oConductanceMatrix, dTimeStep);
	}

	void Resistor::applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) const {
		double dResistance;

		dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeS);
		oConductanceMatrix.setValue(m_iNodeS, m_iNodeS, dResistance + (1.0 / m_dResistance));

		dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeD);
		oConductanceMatrix.setValue(m_iNodeS, m_iNodeD, dResistance - (1.0 / m_dResistance));

		dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeS);
		oConductanceMatrix.setValue(m_iNodeD, m_iNodeS, dResistance - (1.0 / m_dResistance));

		dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeD);
		oConductanceMatrix.setValue(m_iNodeD, m_iNodeD, dResistance + (1.0 / m_dResistance));
	};

	void Resistor::postStep(Matrix& oVoltageMatrix, const double dTimeStep) {
		m_dCurrent = (oVoltageMatrix.getValue(m_iNodeS, 0) - oVoltageMatrix.getValue(m_iNodeD, 0)) / m_dResistance;
	}

}

