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
		m_dVoltageDelta = 0;
	}

	void Inductor::initalize(Matrix& oConductanceMatrix, const double dTimeStep) {
		m_dCurrent = 0;
		m_dVoltageDelta = 0;
		applyConductanceMatrixStamp(oConductanceMatrix, dTimeStep);
	}

	void Inductor::applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) {
		double dResistance;

		m_dComponentResistanceStamp = dTimeStep / (2.0 * m_dInductance);

		dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeS);
		oConductanceMatrix.setValue(m_iNodeS, m_iNodeS, dResistance + m_dComponentResistanceStamp);

		dResistance = oConductanceMatrix.getValue(m_iNodeS, m_iNodeD);
		oConductanceMatrix.setValue(m_iNodeS, m_iNodeD, dResistance - m_dComponentResistanceStamp);

		dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeS);
		oConductanceMatrix.setValue(m_iNodeD, m_iNodeS, dResistance - m_dComponentResistanceStamp);

		dResistance = oConductanceMatrix.getValue(m_iNodeD, m_iNodeD);
		oConductanceMatrix.setValue(m_iNodeD, m_iNodeD, dResistance + m_dComponentResistanceStamp);
	};

	void Inductor::applySourceVectorMatrixStamp(Matrix& oSourceVector) {
		double dCurrent;

		m_dCurrent = m_dComponentResistanceStamp * m_dVoltageDelta + m_dCurrent; // dt/2L*v(t-1) + i(t-1)

		dCurrent = oSourceVector.getValue(m_iNodeS, 0);
		oSourceVector.setValue(m_iNodeS, 0, dCurrent - m_dCurrent);

		dCurrent = oSourceVector.getValue(m_iNodeD, 0);
		oSourceVector.setValue(m_iNodeD, 0, dCurrent + m_dCurrent);
	};

	void Inductor::step(Matrix& oSourceVector) { // Trapezoidal integration
		applySourceVectorMatrixStamp(oSourceVector);
	}

	void Inductor::postStep(Matrix& oVoltageMatrix) {
		m_dVoltageDelta = (oVoltageMatrix.getValue(m_iNodeS, 0) - oVoltageMatrix.getValue(m_iNodeD, 0));
		m_dCurrent = m_dComponentResistanceStamp * m_dVoltageDelta + m_dCurrent; // i(t) = dt/2L*v(t) + dt/2L*v(t-1) + i(t-1), m_dCurrent = dt/2L*v(t-1) + i(t-1)
	}

}
