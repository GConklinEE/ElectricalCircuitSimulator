#include "Inductor.h"

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

	Inductor::Inductor(const int iNodeS, const int iNodeD, const double dInductance)
	: CircuitComponent(iNodeS, iNodeD, false) {
		if (dInductance <= 0) {
			cout << "Inductance value must be greater than 0!" << endl;
			throw invalid_argument("Inductance value must be greater than 0!");
		}
		m_dInductance = dInductance;
		m_dVoltageDeltaT = 0;
		m_dVoltageDeltaTM1 = 0;
	}

	void Inductor::initalize(Matrix& oConductanceMatrix, const double dTimeStep) {
		applyConductanceMatrixStamp(oConductanceMatrix, dTimeStep);
	}

	void Inductor::applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) const {
		double dResistance;
		double dComponentResistance = dTimeStep / (2.0 * m_dInductance);

		dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeS);
		oConductanceMatrix.setValue(m_iNodeS, m_iNodeS, dResistance + dComponentResistance);

		dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeD);
		oConductanceMatrix.setValue(m_iNodeS, m_iNodeD, dResistance - dComponentResistance);

		dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeS);
		oConductanceMatrix.setValue(m_iNodeD, m_iNodeS, dResistance - dComponentResistance);

		dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeD);
		oConductanceMatrix.setValue(m_iNodeD, m_iNodeD, dResistance + dComponentResistance);
	};

	void Inductor::step(Matrix& oSourceVector, const double dTimeStep) { // Trapezoidal integration
		double dCurrent;

		m_dCurrent = dTimeStep / (2.0 * m_dInductance) * (m_dVoltageDeltaT + m_dVoltageDeltaTM1) + m_dCurrent;
		dCurrent = oSourceVector.getValue(m_iNodeD, 0);
		oSourceVector.setValue(m_iNodeD, 0, dCurrent + m_dCurrent);
		dCurrent = oSourceVector.getValue(m_iNodeS, 0);
		oSourceVector.setValue(m_iNodeS, 0, dCurrent - m_dCurrent);
	}

	void Inductor::postStep(Matrix& oVoltageMatrix, const double dTimeStep) {
		m_dVoltageDeltaTM1 = m_dVoltageDeltaT;
		m_dVoltageDeltaT = (oVoltageMatrix.getValue(m_iNodeS, 0) - oVoltageMatrix.getValue(m_iNodeD, 0));
	}

}
