#include "CircuitComponent.h"

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

	CircuitComponent::CircuitComponent(const int iNodeS, const int iNodeD, bool bIsGround) {
		if ((iNodeS < 0) || (iNodeD < 0)) {
			cout << "Node values must be greater than or equal to 0!" << endl;
			throw invalid_argument("Node values must be greater than or equal to 0!");
		}
		if (iNodeS == iNodeD) {
			cout << "Node values must not be the same!" << endl;
			throw invalid_argument("Node values must not be the same!");
		}
		m_bIsGround = bIsGround;
		m_iNodeS = iNodeS;
		m_iNodeD = iNodeD;
		m_dCurrent = 0.0;
	}

	void CircuitComponent::initalize(Matrix& oConductanceMatrix, const double dTimeStep) {
		;
	}

	void CircuitComponent::step(Matrix& oSourceVector, const double dTimeStep) {
		;
	}

	void CircuitComponent::postStep(Matrix& oVoltageMatrix, const double dTimeStep) {
		;
	}

	void CircuitComponent::applyConductanceMatrixStamp(Matrix& oConductanceMatrix, const double dTimeStep) const {
		;
	}

}