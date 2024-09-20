#include "LinearCircuit.h"
#include <iostream>

using std::cout;
using std::endl;
using std::invalid_argument;
using std::exception;
using std::move;
using std::vector;
using std::unique_ptr;
using std::make_unique;

namespace SimulationEngine {

    LinearCircuit::LinearCircuit(const size_t iNumComponents) :
        m_iMaxComponentCount(iNumComponents),
        m_iComponentCount(0),
        m_iMaxNode(0),
        m_iGroundNode(0),
        m_dStopTime(0),
        m_dTimeStep(0),
        m_dTime(0),
        m_bHasGround(false),
        m_bInitSim(false),
        m_bRunSim(false),
        m_pCircuitComponents(make_unique<unique_ptr<CircuitComponent>[]>(m_iMaxComponentCount))
    {
        if (iNumComponents <= 0) {
            cout << "Circuit must have a positive and non-zero number of components!" << endl;
            throw exception("Circuit must have a positive and non-zero number of components!");
        }
    }

    size_t LinearCircuit::addComponent(unique_ptr<CircuitComponent> pCircuitComponent) {
        size_t iNodeIterator;
        size_t iNodeS = pCircuitComponent->getNodeS();
        size_t iNodeD = pCircuitComponent->getNodeD();
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
            m_pCircuitComponents[m_iComponentCount] = move(pCircuitComponent);
            m_iComponentCount++;
        }
        for (iNodeIterator = 0; iNodeIterator < static_cast<int>(m_oNodeList.size()); iNodeIterator++) {
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

    double LinearCircuit::getVoltage(const size_t iNode) const {
        if (iNode > m_iMaxNode) {
            cout << "Requested node does not exist!" << endl;
            throw invalid_argument("Requested node does not exist!");
        }
        if (m_bRunSim == false) {
            cout << "Cannot read voltages from a circuit that has not been simulated!" << endl;
            throw exception("Cannot read voltages from a circuit that has not been simulated!");
        }

        return m_oVoltageVector(iNode, 0);
    }

    double LinearCircuit::getCurrent(const size_t iComponentIndex) const {
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
        size_t iIterator;
        size_t iNode;
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
        m_oConductanceMatrix = Matrix<double>(m_iMaxNode + 1, m_iMaxNode + 1);
        m_oSourceVector = Matrix<double>(m_iMaxNode + 1, 1);
        m_oVoltageVector = Matrix<double>(m_iMaxNode + 1, 1);

        // Build the conductance and initial source vector matrices
        for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
            m_pCircuitComponents[iIterator]->initalize(m_oConductanceMatrix, m_dTimeStep);
        }

        // Factor the conductance matrix
        m_oPLU = PLU_Factorization<double>(m_oConductanceMatrix);

#ifdef MATRIX_PRINT
        // Print out the matrices
        cout << "Conductance Matrix:" << endl;
        cout << m_oConductanceMatrix.getMatrixString();
        cout << "Source Vector:" << endl;
        cout << m_oSourceVector.getMatrixString();
        cout << "PLU Factorization Matrixes:" << endl;
        cout << "L:" << endl;
        cout << m_oPLU.getL().getMatrixString();
        cout << "P:" << endl;
        cout << m_oPLU.getP().getMatrixString();
        cout << "Q:" << endl;
        cout << m_oPLU.getQ().getMatrixString();
        cout << "U:" << endl;
        cout << m_oPLU.getU().getMatrixString();
#endif

        m_dTime = 0; // Set simulation time to zero
        m_bInitSim = true; // Simulation has been initalized
    }

    bool LinearCircuit::step() {
        size_t iIterator;
        double dNormalizationFactor;

        if (m_bInitSim == false) {
            cout << "Circuit has not been initalized!" << endl;
            throw exception("Circuit has not been initalized!");
        }

#ifdef MATRIX_PRINT
        cout << "**** Time: " << (m_dTime + m_dTimeStep) << " s ****\n" << endl;
#endif

        m_oSourceVector.clear(); // Is rebuilt every step

        // Run all component step functions
        for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
            m_pCircuitComponents[iIterator]->step(m_oSourceVector);
        }

        // Find the new voltage matrix
        m_oVoltageVector = m_oPLU.solve(m_oSourceVector);

        // Check to see if the voltage matrix requires normalization
        dNormalizationFactor = -m_oVoltageVector(m_iGroundNode);
        if (dNormalizationFactor != 0) {
            for (iIterator = 0; iIterator <= m_iMaxNode; iIterator++) {
                m_oVoltageVector(iIterator) = m_oVoltageVector(iIterator) + dNormalizationFactor;
            }
        }

        // Run all component post-step functions 
        for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
            m_pCircuitComponents[iIterator]->postStep(m_oVoltageVector);
        }

        m_dTime += m_dTimeStep; // Update simulation runtime
        m_bRunSim = true; // Simulation has been run at least 1 time step

#ifdef MATRIX_PRINT
        cout << "Source Vector:" << endl;
        cout << m_oSourceVector.getMatrixString();
        cout << "Voltage Matrix:" << endl;
        cout << m_oVoltageVector.getMatrixString();
#endif

        if (m_dTime >= m_dStopTime)
            return true; // Simulation is done
        else
            return false;
    }

}

