#include "LinearCircuit.h"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::exception;

namespace SimulationEngine {

	LinearCircuit::LinearCircuit(const int iNumComponents) {
		m_iMaxComponentCount = iNumComponents;
		m_oCircuitComponents = new CircuitComponent*[m_iMaxComponentCount]; // We have to use statically allocated arrays of objects due to using the C++ CLI/CLR
		m_oConductanceMatrix = Matrix();
		m_oSourceVector = Matrix();
		m_oVoltageMatrix = Matrix();
		m_oPLU_Factorization = PLU_Factorization();
		m_oNodeList = vector<int>(0);
		m_bHasGround = false;
		m_iComponentCount = 0;
		m_iNodeCount = 0;
		m_dStopTime = 0;
		m_dTimeStep = 0;
		m_dTime = 0;
		m_bInitSim = false;
		m_bRunSim = false;
	}

	LinearCircuit::~LinearCircuit() {
		int iComponentIterator;

		for (iComponentIterator = 0; iComponentIterator < m_iMaxComponentCount; iComponentIterator++) {
			if (m_oCircuitComponents[iComponentIterator] != nullptr) {
				delete m_oCircuitComponents[iComponentIterator];
			}
		}
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
			}
			m_oCircuitComponents[m_iComponentCount] = pCircuitComponent;
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
			m_iNodeCount++;
		}
		if (!bFoundNodeD) {
			m_oNodeList.push_back(iNodeD);
			m_iNodeCount++;
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
		if (m_bRunSim == false) {
			cout << "Cannot read voltages from a circuit that has not been simulated!" << endl;
			throw exception("Cannot read voltages from a circuit that has not been simulated!");
		}

		return m_oVoltageMatrix.getValue(iNode, 0);
	}

	double LinearCircuit::getCurrent(const int iComponentIndex) const {
		if (iComponentIndex < m_iComponentCount) {
			cout << "Requested component does not exist!" << endl;
			throw invalid_argument("Requested component does not exist!");
		}
		if (m_bRunSim == false) {
			cout << "Cannot read currents from a circuit that has not been simulated!" << endl;
			throw exception("Cannot read currents from a circuit that has not been simulated!");
		}

		return m_oCircuitComponents[iComponentIndex]->getCurrent();
	}

	void LinearCircuit::initalize() {
		int iComponentIterator;

		// Check for valid time step and stop time. Check that there are actually nodes in the circuit to simulate. Check that a ground exists.
		if (m_dStopTime == 0) {
			cout << "Stop time cannot be zero!" << endl;
			throw exception("Stop time cannot be zero!");
		}
		if (m_dTimeStep == 0) {
			cout << "Time step cannot be zero!" << endl;
			throw exception("STime step cannot be zero!");
		}
		if (m_dStopTime < m_dTimeStep) {
			cout << "Stop time cannot be smaller than time step!" << endl;
			throw exception("Stop time cannot be smaller than time step!");
		}
		if (m_iNodeCount < 2) {
			cout << "There are no components in the circuit!" << endl;
			throw exception("There are no components in the circuit!");
		}
		if (m_bHasGround == false) {
			cout << "There is no ground in the circuit!" << endl;
			throw exception("There is no ground in the circuit!");
		}

		// Declare blank matrices.
		m_oConductanceMatrix = Matrix(m_iNodeCount, m_iNodeCount);
		m_oSourceVector = Matrix(m_iNodeCount, 1);
		m_oVoltageMatrix = Matrix(m_iNodeCount, 1);

		// Build the conductance and initial source vector matrices
		for (iComponentIterator = 0; iComponentIterator < m_iComponentCount; iComponentIterator++) {
			m_oCircuitComponents[iComponentIterator]->initalize(m_oConductanceMatrix, m_dTimeStep);
		}

#ifdef MATRIX_PRINT
		// Print out the matrices
		cout << "Conductance Matrix:" << endl;
		m_oConductanceMatrix.printMatrix();
		cout << "Source Vector:" << endl;
		m_oSourceVector.printMatrix();
#endif

		// Factor the conductance matrix
		m_oPLU_Factorization = m_oConductanceMatrix.runPLU_Factorization();

		// Set simulation time to zero
		m_dTime = 0;

		m_bInitSim = true;
	}

	bool LinearCircuit::step() {
		int iComponentIterator;

		if (m_bInitSim == false) {
			cout << "Circuit has not been initalized!" << endl;
			throw exception("Circuit has not been initalized!");
		}

		m_oSourceVector.clear(); // Is rebuilt every step

		// Run all component step functions
		for (iComponentIterator = 0; iComponentIterator < m_iComponentCount; iComponentIterator++) {
			m_oCircuitComponents[iComponentIterator]->step(m_oSourceVector, m_dTimeStep);
		}
	
		// Find the new voltage matrix
		m_oVoltageMatrix = Matrix::linearSystemSolver(m_oSourceVector, m_oPLU_Factorization);

		// Run all component post-step functions 
		for (iComponentIterator = 0; iComponentIterator < m_iComponentCount; iComponentIterator++) {
			m_oCircuitComponents[iComponentIterator]->postStep(m_oVoltageMatrix, m_dTimeStep);
		}

		m_dTime += m_dTimeStep;
		m_bRunSim = true;

#ifdef MATRIX_PRINT
		cout << "Time: " << m_dTime << " s" << endl;
		cout << "Voltage Matrix:" << endl;
		m_oVoltageMatrix.printMatrix();
#endif

		if (m_dTime >= m_dStopTime)
			return true; // Simulation is done
		else
			return false;
	}

	}

