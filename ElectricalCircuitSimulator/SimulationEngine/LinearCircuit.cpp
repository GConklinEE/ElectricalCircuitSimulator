#include "LinearCircuit.h"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::exception;

namespace SimulationEngine {

	LinearCircuit::LinearCircuit(const int iNumComponents) {
		if (iNumComponents <= 0) {
			cout << "Circuit must have a positive and non-zero number of components!" << endl;
			throw exception("Circuit must have a positive and non-zero number of components!");
		}
		m_iMaxComponentCount = iNumComponents;
		m_pCircuitComponents = new CircuitComponent*[m_iMaxComponentCount]; // We have to use statically allocated arrays of objects due to using the C++ CLI/CLR
		m_pConductanceMatrix = new Matrix();
		m_pSourceVector = new Matrix();
		m_pVoltageMatrix = new Matrix();
		m_pPLU_Factorization = new PLU_Factorization();
		m_oNodeList = vector<int>(0);
		m_bHasGround = false;
		m_iComponentCount = 0;
		m_iMaxNode = 0;
		m_iGroundNode = 0;
		m_dStopTime = 0;
		m_dTimeStep = 0;
		m_dTime = 0;
		m_bInitSim = false;
		m_bRunSim = false;
	}

	LinearCircuit::~LinearCircuit() {
		delete[] m_pCircuitComponents;
		delete m_pConductanceMatrix;
		delete m_pSourceVector;
		delete m_pVoltageMatrix;
		delete m_pPLU_Factorization;
	}

	int LinearCircuit::addComponent(CircuitComponent* pCircuitComponent) {
		int iNodeIterator;
		int iNodeS = pCircuitComponent->getNodeS();
		int iNodeD = pCircuitComponent->getNodeD();
		bool bFoundNodeS = false;
		bool bFoundNodeD = false;

		if (m_iComponentCount == m_iMaxComponentCount) {
			cout << "Circuit is already full of components!" << endl;
			throw exception("Circuit is already full of components!");
		}
		if ((pCircuitComponent->getIsGround() == true) && (m_bHasGround == true)) {
			cout << "Circuit already has a ground, cannot add another one!" << endl;
			throw exception("Circuit already has a ground, cannot add another one!");
		} else {
			// We are changing the circuit
			m_bInitSim = false;
			m_bRunSim = false;
			if (pCircuitComponent->getIsGround() == true) {
				m_bHasGround = true;
				m_iGroundNode = pCircuitComponent->getNodeS();
			}
			m_pCircuitComponents[m_iComponentCount] = pCircuitComponent;
			m_iComponentCount++;
		}
		for (iNodeIterator = 0; iNodeIterator < (int)m_oNodeList.size(); iNodeIterator++) {
			if (!bFoundNodeS && (iNodeS == m_oNodeList[iNodeIterator])) {
				bFoundNodeS = true;
			}
			if (!bFoundNodeD && (iNodeD == m_oNodeList[iNodeIterator])) {
				bFoundNodeD = true;
			}
			if ((bFoundNodeS == true) && (bFoundNodeD == true)) {
				break;
			}
		}
		if (!bFoundNodeS) {
			m_oNodeList.push_back(iNodeS);
			if (iNodeS > m_iMaxNode) {
				m_iMaxNode = iNodeS;
			}
		}
		if (!bFoundNodeD) {
			m_oNodeList.push_back(iNodeD);
			if (iNodeD > m_iMaxNode) {
				m_iMaxNode = iNodeD;
			}
		}
		return m_iComponentCount - 1; // Index of added component
	};

	void LinearCircuit::setStopTime(const double dStopTime) {
		if (dStopTime <= 0) {
			cout << "Stop time must be greater than 0!" << endl;
			throw invalid_argument("Stop time must be greater than 0!");
		}
		m_dStopTime = dStopTime;
	}

	void LinearCircuit::setTimeStep(const double dTimeStep) {
		if (dTimeStep <= 0) {
			cout << "Time step must be greater than 0!" << endl;
			throw invalid_argument("Time step must be greater than 0!");
		}
		m_dTimeStep = dTimeStep;
	}

	double LinearCircuit::getTime() const {
		return m_dTime;
	}

	double LinearCircuit::getVoltage(const int iNode) const {
		if (iNode < 0) {
			cout << "Node values must be greater than or equal to 0!" << endl;
			throw invalid_argument("Node values must be greater than or equal to 0!");
		}
		if (iNode > m_iMaxNode) {
			cout << "Requested node does not exist!" << endl;
			throw invalid_argument("Requested node does not exist!");
		}
		if (m_bRunSim == false) {
			cout << "Cannot read voltages from a circuit that has not been simulated!" << endl;
			throw exception("Cannot read voltages from a circuit that has not been simulated!");
		}

		return m_pVoltageMatrix->getValue(iNode, 0);
	}

	double LinearCircuit::getCurrent(const int iComponentIndex) const {
		if (iComponentIndex >= m_iComponentCount) {
			cout << "Requested component does not exist!" << endl;
			throw invalid_argument("Requested component does not exist!");
		}
		if (m_bRunSim == false) {
			cout << "Cannot read currents from a circuit that has not been simulated!" << endl;
			throw exception("Cannot read currents from a circuit that has not been simulated!");
		}

		return m_pCircuitComponents[iComponentIndex]->getCurrent();
	}

	void LinearCircuit::initalize() {
		int iIterator;
		int iNode;
		bool bFoundIndex;

		// Check for valid time step and stop time. Check that there are actually nodes in the circuit to simulate. Check that a ground exists.
		if (m_dStopTime < m_dTimeStep) {
			cout << "Stop time cannot be smaller than time step!" << endl;
			throw exception("Stop time cannot be smaller than time step!");
		}
		if (m_iMaxNode == 0) {
			cout << "There are no components in the circuit!" << endl;
			throw exception("There are no components in the circuit!");
		}
		if (m_bHasGround == false) {
			cout << "There is no ground in the circuit!" << endl;
			throw exception("There is no ground in the circuit!");
		}
		for (iNode = 0; iNode < m_oNodeList.size(); iNode++) {
			bFoundIndex = false;
			for (iIterator = 0; iIterator < m_oNodeList.size(); iIterator++) {
				if (m_oNodeList[iIterator] == iNode) {
					bFoundIndex = true;
					break;
				}
			}
			if (bFoundIndex == false) {
				cout << "Circuit nodes are not condensed into the smallest matrix possible!" << endl;
				throw exception("Circuit nodes are not condensed into the smallest matrix possible!");
				break;
			}
		}

		// Declare blank matrices.
		m_pConductanceMatrix = new Matrix(m_iMaxNode + 1, m_iMaxNode + 1);
		m_pSourceVector = new Matrix(m_iMaxNode + 1, 1);
		m_pVoltageMatrix = new Matrix(m_iMaxNode + 1, 1);

		// Build the conductance and initial source vector matrices
		for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
			m_pCircuitComponents[iIterator]->initalize(*m_pConductanceMatrix, m_dTimeStep);
		}

		// Factor the conductance matrix
		m_pPLU_Factorization = m_pConductanceMatrix->runPLU_Factorization();

#ifdef MATRIX_PRINT
		// Print out the matrices
		cout << "Conductance Matrix:" << endl;
		m_pConductanceMatrix->printMatrix();
		cout << "Source Vector:" << endl;
		m_pSourceVector->printMatrix();
		cout << "PLU Factorization Matrixes:" << endl;
		cout << "L:" << endl;
		m_pPLU_Factorization->m_pL->printMatrix();
		cout << "P:" << endl;
		m_pPLU_Factorization->m_pP->printMatrix();
		cout << "Q:" << endl;
		m_pPLU_Factorization->m_pQ->printMatrix();
		cout << "U:" << endl;
		m_pPLU_Factorization->m_pU->printMatrix();
#endif

		// Set simulation time to zero
		m_dTime = 0;

		m_bInitSim = true;
	}

	bool LinearCircuit::step() {
		int iIterator;
		double dNormalizationFactor;

		if (m_bInitSim == false) {
			cout << "Circuit has not been initalized!" << endl;
			throw exception("Circuit has not been initalized!");
		}

#ifdef MATRIX_PRINT
		cout << "**** Time: " << (m_dTime + m_dTimeStep) << " s ****\n" << endl;
#endif

		m_pSourceVector->clear(); // Is rebuilt every step

		// Run all component step functions
		for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
			m_pCircuitComponents[iIterator]->step(*m_pSourceVector);
		}
	
		// Find the new voltage matrix
		m_pVoltageMatrix = Matrix::linearSystemSolver(*m_pSourceVector, *m_pPLU_Factorization);

		// Check to see if the voltage matrix requires normalization
		dNormalizationFactor = -m_pVoltageMatrix->getValue(m_iGroundNode, 0);
		if (dNormalizationFactor != 0) {
			for (iIterator = 0; iIterator <= m_iMaxNode; iIterator++) {
				m_pVoltageMatrix->setValue(iIterator, 0, (m_pVoltageMatrix->getValue(iIterator, 0) + dNormalizationFactor));
			}
		}

		// Run all component post-step functions 
		for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
			m_pCircuitComponents[iIterator]->postStep(*m_pVoltageMatrix);
		}

		m_dTime += m_dTimeStep;
		m_bRunSim = true;

#ifdef MATRIX_PRINT
		cout << "Source Vector:" << endl;
		m_pSourceVector->printMatrix();
		cout << "Voltage Matrix:" << endl;
		m_pVoltageMatrix->printMatrix();
#endif

		if (m_dTime >= m_dStopTime)
			return true; // Simulation is done
		else
			return false;
	}

}

