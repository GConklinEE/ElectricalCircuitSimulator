#include "LinearCircuit.h"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::exception;
using std::move;
using std::vector;
using std::unique_ptr;
using std::make_unique;

#define ZERO (1e-9) // If a diagonal on the U matrix is less than or equal to this, it is considered a zero

namespace SimulationEngine {

    LinearCircuit::LinearCircuit(const size_t iNumComponents) :
        m_pConductanceMatrix(make_unique<Matrix>()),
        m_pSourceVector(make_unique<Matrix>()),
        m_pVoltageVector(make_unique<Matrix>()),
        m_pPLU(make_unique<PLU_Factorization>()),
        m_oNodeList(vector<size_t>(0)),
        m_bHasGround(false),
        m_iComponentCount(0),
        m_iMaxNode(0),
        m_iGroundNode(0),
        m_dStopTime(0),
        m_dTimeStep(0),
        m_dTime(0),
        m_bInitSim(false),
        m_bRunSim(false)
    {
        if (iNumComponents <= 0) {
            cout << "Circuit must have a positive and non-zero number of components!" << endl;
            throw exception("Circuit must have a positive and non-zero number of components!");
        }
        m_iMaxComponentCount = iNumComponents;
        m_pCircuitComponents = make_unique<unique_ptr<CircuitComponent>[]>(m_iMaxComponentCount); // We have to use statically allocated arrays of objects due to using the C++ CLI/CLR
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

    double LinearCircuit::getVoltage(const size_t iNode) const {
        if (iNode > m_iMaxNode) {
            cout << "Requested node does not exist!" << endl;
            throw invalid_argument("Requested node does not exist!");
        }
        if (m_bRunSim == false) {
            cout << "Cannot read voltages from a circuit that has not been simulated!" << endl;
            throw exception("Cannot read voltages from a circuit that has not been simulated!");
        }

        return (*m_pVoltageVector)(iNode, 0);
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
        m_pConductanceMatrix = make_unique<Matrix>(m_iMaxNode + 1, m_iMaxNode + 1);
        m_pSourceVector = make_unique<Matrix>(m_iMaxNode + 1, 1);
        m_pVoltageVector = make_unique<Matrix>(m_iMaxNode + 1, 1);

        // Build the conductance and initial source vector matrices
        for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
            m_pCircuitComponents[iIterator]->initalize(*m_pConductanceMatrix, m_dTimeStep);
        }

        // Factor the conductance matrix
        m_pPLU = make_unique<PLU_Factorization>(*m_pConductanceMatrix);

#ifdef MATRIX_PRINT
        // Print out the matrices
        cout << "Conductance Matrix:" << endl;
        cout << m_pConductanceMatrix->printMatrix();
        cout << "Source Vector:" << endl;
        cout << m_pSourceVector->printMatrix();
        cout << "PLU Factorization Matrixes:" << endl;
        cout << "L:" << endl;
        cout << m_pPLU->getL().printMatrix();
        cout << "P:" << endl;
        cout << m_pPLU->getP().printMatrix();
        cout << "Q:" << endl;
        cout << m_pPLU->getQ().printMatrix();
        cout << "U:" << endl;
        cout << m_pPLU->getU().printMatrix();
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

        m_pSourceVector->clear(); // Is rebuilt every step

        // Run all component step functions
        for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
            m_pCircuitComponents[iIterator]->step(*m_pSourceVector);
        }

        // Find the new voltage matrix
        solver();

        // Check to see if the voltage matrix requires normalization
        dNormalizationFactor = -(*m_pVoltageVector)(m_iGroundNode, 0);
        if (dNormalizationFactor != 0) {
            for (iIterator = 0; iIterator <= m_iMaxNode; iIterator++) {
                (*m_pVoltageVector)(iIterator, 0) = ((*m_pVoltageVector)(iIterator, 0) + dNormalizationFactor);
            }
        }

        // Run all component post-step functions 
        for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
            m_pCircuitComponents[iIterator]->postStep(*m_pVoltageVector);
        }

        m_dTime += m_dTimeStep; // Update simulation runtime
        m_bRunSim = true; // Simulation has been run at least 1 time step

#ifdef MATRIX_PRINT
        cout << "Source Vector:" << endl;
        m_pSourceVector->printMatrix();
        cout << "Voltage Matrix:" << endl;
        m_pVoltageVector->printMatrix();
#endif

        if (m_dTime >= m_dStopTime)
            return true; // Simulation is done
        else
            return false;
    }

    void LinearCircuit::solver() {
        int iI1;
        size_t iI2;
        size_t iIndex;
        size_t iNumRows = m_pSourceVector->getNumRows();
        Matrix oX(iNumRows, 1); // UX = Y
        Matrix oY(iNumRows, 1); // LY = B_Permuted
        Matrix oB_Permuted(iNumRows, 1);

        // Apply row permutations to B to get B_Permuted
        for (iI1 = 0; iI1 < iNumRows; ++iI1) {
            iIndex = (size_t)round(m_pPLU->getP()(iI1, 0));
            oB_Permuted(iI1, 0) = (*m_pSourceVector)(iIndex, 0);
        }

        // Forward substitution to solve LY = B_Permuted
        for (iI1 = 0; iI1 < iNumRows; ++iI1) {
            oY(iI1, 0) = oB_Permuted(iI1, 0);
            for (iI2 = 0; iI2 < iI1; ++iI2) {
                oY(iI1, 0) -= m_pPLU->getL()(iI1, iI2) * oY(iI2, 0);
            }
        }

        // Backward substitution to solve UX = Y
        for (iI1 = (int)iNumRows - 1; iI1 >= 0; --iI1) {
            oX(iI1, 0) = oY(iI1, 0);
            for (iI2 = iI1 + 1; iI2 < iNumRows; ++iI2) {
                oX(iI1, 0) -= m_pPLU->getU()(iI1, iI2) * oX(iI2, 0);
            }
            if (fabs(m_pPLU->getU()(iI1, iI1)) > ZERO) {
                oX(iI1, 0) /= m_pPLU->getU()(iI1, iI1);
            } else {
                oX(iI1, 0) = 0; // If a diagonal on the U matrix is 0, it's due to the ground node being included in the matrix, and is effectively infinity, resulting in oX = 0 for this row.
            }
        }

        // Apply column permutations to X using Q to get X_Final
        for (iI1 = 0; iI1 < iNumRows; ++iI1) {
            iIndex = (size_t)round(m_pPLU->getQ()(iI1, 0));
            (*m_pVoltageVector)(iIndex, 0) = oX(iI1, 0);
        }

#ifdef MATRIX_PRINT
        cout << "X Vector:" << endl;
        cout << oX.printMatrix();
        cout << "Y Vector:" << endl;
        cout << oY.printMatrix();
        cout << "B Permuted Vector:" << endl;
        cout << oB_Permuted.printMatrix();
#endif
    }

}

