#pragma once

#include "Component.h"
#include "PLU_Factorization.h"
#include "Matrix.h"
#include <vector>

namespace SimulationEngine {

    #pragma region Concepts

    template<class T>
    concept DiscreteEventTimeDomainSimComponentGeneral = requires(T t) {
        { T(std::move(t)) } -> std::same_as<T>; // Move constructor
        { t = std::move(t) } -> std::same_as<T&>; // Move assignment operator
    };

    template<class T>
    concept DiscreteEventTimeDomainSimComponentInitalize = requires(T t, const double dTimeStep) {
        { t.DETDS_initalize(dTimeStep) } -> std::same_as<void>;
    };

    template<class T>
    concept DiscreteEventTimeDomainSimComponentStep = requires(T t) {
        { t.DETDS_step() } -> std::same_as<void>;
    };

    template<class T>
    concept NodeSimComponentGeneral = requires(T t, const int iNodeIndex) {
        { t.getNode(iNodeIndex) } -> std::same_as<size_t>;
        { t.getNumNodes() } -> std::same_as<size_t>;
    };

    template<class T>
    concept LinearNaturalSimComponentGeneral = requires(T t) {
        { t.getThrough() } -> std::same_as<double>;
        { t.hasAcrossReferenceNode() } -> std::same_as<bool>;
        { t.getAcrossReferenceNode() } -> std::same_as<size_t>;
    };

    template<class T>
    concept LinearNaturalSimComponentInitalize = requires(T t, Matrix<double>&oMatrix, const double dTimeStep) {
        { t.LNS_initalize(oMatrix, dTimeStep) } -> std::same_as<void>;
    };

    template<class T>
    concept LinearNaturalSimComponentStep = requires(T t, Matrix<double>&oMatrix, const double dTimeStep) {
        { t.LNS_step(oMatrix) } -> std::same_as<void>;
    };

    template<class T>
    concept LinearNaturalSimComponentPostStep = requires(T t, Matrix<double>&oMatrix, const double dTimeStep) {
        { t.LNS_postStep(oMatrix) } -> std::same_as<void>;
    };

    template<class T>
    concept LinearCircuitSimComponentGeneral = requires(T t) {
        { t.getCurrent() } -> std::same_as<double>;
        { t.hasGroundNode() } -> std::same_as<bool>;
    };

    #pragma endregion

    template<class T>
    requires DiscreteEventTimeDomainSimComponentGeneral<T> &&
             DiscreteEventTimeDomainSimComponentInitalize<T> &&
             DiscreteEventTimeDomainSimComponentStep<T>
    class DiscreteEventTimeDomainSimulation {

        public:

            #pragma region Constructors

            DiscreteEventTimeDomainSimulation(const size_t iNumComponents) :
                m_iMaxComponentCount(iNumComponents),
                m_iComponentCount(0),
                m_dStopTime(0),
                m_dTime(0),
                m_dTimeStep(0),
                m_bInitSim(false),
                m_bRunSim(false),
                m_pComponents(std::make_unique<std::unique_ptr<T>[]>(m_iMaxComponentCount))
            {
                if (iNumComponents <= 0) {
                    std::cout << "Simulation must have a positive and non-zero number of components!" << std::endl;
                    throw std::exception("Simulation must have a positive and non-zero number of components!");
                }
            }

            #pragma endregion

            #pragma region Observers

            double getTime() const {
                return m_dTime;
            }

            #pragma endregion

            #pragma region Public Modifiers

            virtual size_t addComponent(std::unique_ptr<T> pComponent) {
                if (m_iComponentCount == m_iMaxComponentCount) {
                    std::cout << "Simulation is already full of components!" << std::endl;
                    throw std::exception("Simulation is already full of components!");
                }

                // We are changing the simulation
                m_bInitSim = false;
                m_bRunSim = false;

                m_pComponents[m_iComponentCount] = move(pComponent);
                m_iComponentCount++;

                return m_iComponentCount - 1; // Index of added component
            };

            void setStopTime(const double dStopTime) {
                if (dStopTime <= 0) {
                    std::cout << "Stop time must be greater than 0!" << std::endl;
                    throw std::invalid_argument("Stop time must be greater than 0!");
                }
                m_dStopTime = dStopTime;
            }

            void setTimeStep(const double dTimeStep) {
                if (dTimeStep <= 0) {
                    std::cout << "Time step must be greater than 0!" << std::endl;
                    throw std::invalid_argument("Time step must be greater than 0!");
                }
                m_dTimeStep = dTimeStep;
            }

            virtual void initalize(bool bInitComponents) {
                size_t iIterator;

                m_dTime = 0; // Set simulation time to zero
                m_bInitSim = true; // Simulation has been initalized

                if (m_dStopTime < m_dTimeStep) {
                    std::cout << "Stop time cannot be smaller than time step!" << std::endl;
                    throw std::exception("Stop time cannot be smaller than time step!");
                }
                if (m_iComponentCount == 0) {
                    std::cout << "There are no components in the Simulation!" << std::endl;
                    throw std::exception("There are no components in the Simulation!");
                }

                if (bInitComponents) {
                    for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
                        m_pComponents[iIterator]->DETDS_initalize(m_dTimeStep);
                    }
                }
            }

            virtual bool step() {
                size_t iIterator;

                stepStart();

                // Run all component step functions
                for (iIterator = 0; iIterator < m_iComponentCount; iIterator++) {
                    m_pComponents[iIterator]->DETDS_step();
                }

                return stepEnd();
            }

            #pragma endregion

        protected:

            #pragma region Protected Modifiers

            virtual void stepStart() {
                if (m_bInitSim == false) {
                    std::cout << "Simulation has not been initalized!" << std::endl;
                    throw std::exception("Simulation has not been initalized!");
                }

#ifdef MATRIX_PRINT
                std::cout << "**** Time: " << (m_dTime + m_dTimeStep) << " s ****\n" << std::endl;
#endif
            }

            virtual bool stepEnd() {
                m_dTime += m_dTimeStep; // Update simulation runtime
                m_bRunSim = true; // Simulation has been run at least 1 time step

                if (m_dTime >= m_dStopTime)
                    return true; // Simulation is done
                else
                    return false;
            }

            #pragma endregion

            #pragma region Members

            size_t m_iMaxComponentCount;
            size_t m_iComponentCount;
            double m_dStopTime;
            double m_dTime;
            double m_dTimeStep;
            bool m_bInitSim;
            bool m_bRunSim;
            std::unique_ptr<std::unique_ptr<T>[]> m_pComponents;

            #pragma endregion
    };

    template<class T>
    requires DiscreteEventTimeDomainSimComponentGeneral<T> &&
             DiscreteEventTimeDomainSimComponentInitalize<T> &&
             DiscreteEventTimeDomainSimComponentStep<T> &&
             NodeSimComponentGeneral<T>
    class NodeSimulation : public DiscreteEventTimeDomainSimulation<T> {

        public:

            #pragma region Constructors

            NodeSimulation(const size_t iNumComponents) :
                DiscreteEventTimeDomainSimulation<T>(iNumComponents),
                m_iMaxNode(0) { ; }

            #pragma endregion

            #pragma region Modifiers

            virtual size_t addComponent(std::unique_ptr<T> pComponent) {
                size_t iComponentNodeIndex;
                size_t iSimNodeIndex;
                size_t iComponentNode;
                bool bFoundNode;

                for (iComponentNodeIndex = 0; iComponentNodeIndex < pComponent->getNumNodes(); iComponentNodeIndex++) {
                    bFoundNode = false;
                    iComponentNode = pComponent->getNode(iComponentNodeIndex);
                    for (iSimNodeIndex = 0; iSimNodeIndex < static_cast<int>(m_oNodeList.size()); iSimNodeIndex++) {
                        if (iComponentNode == m_oNodeList[iSimNodeIndex]) {
                            bFoundNode = true;
                            break;
                        }
                    }
                    if (!bFoundNode) {
                        m_oNodeList.push_back(iComponentNode);
                        if (iComponentNode > m_iMaxNode) {
                            m_iMaxNode = iComponentNode;
                        }
                    }
                }
                return DiscreteEventTimeDomainSimulation<T>::addComponent(std::move(pComponent)); // Index of added component
            };

            virtual void initalize(bool bInitComponents) {
                size_t iNode;
                size_t iIterator;
                bool bFoundIndex;

                for (iNode = 0; iNode < m_oNodeList.size(); iNode++) {
                    bFoundIndex = false;
                    for (iIterator = 0; iIterator < m_oNodeList.size(); iIterator++) {
                        if (m_oNodeList[iIterator] == iNode) {
                            bFoundIndex = true;
                            break;
                        }
                    }
                    if (bFoundIndex == false) {
                        std::cout << "Simulation nodes are not condensed into the smallest number possible!" << std::endl;
                        throw std::exception("Simulation nodes are not condensed into the smallest number possible!");
                        break;
                    }
                }

                DiscreteEventTimeDomainSimulation<T>::initalize(bInitComponents);
            }

            #pragma endregion

        protected:

            #pragma region Members

            size_t m_iMaxNode;
            std::vector<size_t> m_oNodeList;

            #pragma endregion
    };

    template<class T>
    requires DiscreteEventTimeDomainSimComponentGeneral<T> &&
             NodeSimComponentGeneral<T> &&
             LinearNaturalSimComponentGeneral <T> &&
             LinearNaturalSimComponentInitalize <T> &&
             LinearNaturalSimComponentStep <T> &&
             LinearNaturalSimComponentPostStep <T>
    class LinearNaturalSimulation : public NodeSimulation<T> {

        public:

            #pragma region Constructors

            LinearNaturalSimulation(const size_t iNumComponents) :
                NodeSimulation<T>(iNumComponents),
                m_iAcrossReferenceNode(0),
                m_bHasAcrossReferenceNode(false) { ; }

            #pragma endregion

            #pragma region Observers

            double getAcross(const size_t iNode) const {
                if (iNode > this->m_iMaxNode) {
                    std::cout << "Requested node does not exist!" << std::endl;
                    throw std::invalid_argument("Requested node does not exist!");
                }
                if (this->m_bRunSim == false) {
                    std::cout << "Cannot read across values from a simulation that has not been simulated!" << std::endl;
                    throw std::exception("Cannot read across values from a simulation that has not been simulated!");
                }

                return m_oAcrossVector(iNode, 0);
            }

            double getThrough(const size_t iComponentIndex) const {
                if (iComponentIndex >= this->m_iComponentCount) {
                    std::cout << "Requested component does not exist!" << std::endl;
                    throw std::invalid_argument("Requested component does not exist!");
                }
                if (this->m_bRunSim == false) {
                    std::cout << "Cannot read through values from a simulation that has not been simulated!" << std::endl;
                    throw std::exception("Cannot read through values from a simulation that has not been simulated!");
                }

                return this->m_pComponents[iComponentIndex]->getThrough();
            }

            #pragma endregion

            #pragma region Modifiers

            size_t addComponent(std::unique_ptr<T> pComponent) {
                if (pComponent->hasAcrossReferenceNode() == true) {
                    if (m_bHasAcrossReferenceNode == true) {
                        std::cout << "Simulation already has an across reference node, cannot add another one!" << std::endl;
                        throw std::exception("Simulation already has an across reference node, cannot add another one!");
                    } else {
                        m_bHasAcrossReferenceNode = true;
                        m_iAcrossReferenceNode = pComponent->getAcrossReferenceNode();
                    }
                }
                
                return NodeSimulation<T>::addComponent(std::move(pComponent));
            };

            virtual void initalize(bool bInitComponents) {
                size_t iIterator;

                NodeSimulation<T>::initalize(false);

                if (m_bHasAcrossReferenceNode == false) {
                    std::cout << "There is no across reference node in the simulation!" << std::endl;
                    throw std::exception("There is no across reference node in the simulation!");
                }

                // Declare blank matrices.
                m_oSimulationMatrix = Matrix<double>(this->m_iMaxNode + 1, this->m_iMaxNode + 1);
                m_oThroughVector = Matrix<double>(this->m_iMaxNode + 1, 1);
                m_oAcrossVector = Matrix<double>(this->m_iMaxNode + 1, 1);

                // Build the simulation and initial through vector matrices
                if (bInitComponents) {
                    for (iIterator = 0; iIterator < this->m_iComponentCount; iIterator++) {
                        this->m_pComponents[iIterator]->LNS_initalize(m_oSimulationMatrix, this->m_dTimeStep);
                    }
                }

                // Factor the simulation matrix
                m_oPLU = PLU_Factorization<double>(m_oSimulationMatrix);

#ifdef MATRIX_PRINT
                // Print out the matrices
                std::cout << "Simulation Matrix:" << std::endl;
                std::cout << m_oSimulationMatrix.getMatrixString();
                std::cout << "Through Vector:" << std::endl;
                std::cout << m_oThroughVector.getMatrixString();
                std::cout << "PLU Factorization Matrixes:" << std::endl;
                std::cout << "L:" << std::endl;
                std::cout << m_oPLU.getL().getMatrixString();
                std::cout << "P:" << std::endl;
                std::cout << m_oPLU.getP().getMatrixString();
                std::cout << "Q:" << std::endl;
                std::cout << m_oPLU.getQ().getMatrixString();
                std::cout << "U:" << std::endl;
                std::cout << m_oPLU.getU().getMatrixString();
#endif
            }

            virtual bool step() {
                size_t iIterator;
                double dNormalizationFactor;

                DiscreteEventTimeDomainSimulation<T>::stepStart();

                this->m_oThroughVector.clear(); // Is rebuilt every step

                // Run all component step functions
                for (iIterator = 0; iIterator < this->m_iComponentCount; iIterator++) {
                    this->m_pComponents[iIterator]->LNS_step(this->m_oThroughVector);
                }

                // Find the new across vector
                this->m_oAcrossVector = m_oPLU.solve(this->m_oThroughVector);

                // Check to see if the across vector requires normalization
                dNormalizationFactor = -m_oAcrossVector(m_iAcrossReferenceNode);
                if (dNormalizationFactor != 0) {
                    for (iIterator = 0; iIterator <= this->m_iMaxNode; iIterator++) {
                        this->m_oAcrossVector(iIterator) = this->m_oAcrossVector(iIterator) + dNormalizationFactor;
                    }
                }

                // Run all component post-step functions 
                for (iIterator = 0; iIterator < this->m_iComponentCount; iIterator++) {
                    this->m_pComponents[iIterator]->LNS_postStep(this->m_oAcrossVector);
                }

#ifdef MATRIX_PRINT
                std::cout << "Through Vector:" << std::endl;
                std::cout << m_oThroughVector.getMatrixString();
                std::cout << "Across Vector:" << std::endl;
                std::cout << m_oAcrossVector.getMatrixString();
#endif

                return DiscreteEventTimeDomainSimulation<T>::stepEnd();
            }

            #pragma endregion

        protected:

            #pragma region Members

            size_t m_iAcrossReferenceNode;
            bool m_bHasAcrossReferenceNode;
            Matrix<double> m_oSimulationMatrix;
            Matrix<double> m_oAcrossVector;
            Matrix<double> m_oThroughVector;
            PLU_Factorization<double> m_oPLU;

            #pragma endregion
    };

    template<class T>
    requires DiscreteEventTimeDomainSimComponentGeneral<T> &&
             NodeSimComponentGeneral<T> &&
             LinearNaturalSimComponentGeneral <T> &&
             LinearNaturalSimComponentInitalize <T> &&
             LinearNaturalSimComponentStep <T> &&
             LinearNaturalSimComponentPostStep <T> &&
             LinearCircuitSimComponentGeneral<T>
    class LinearCircuitSimulation : public LinearNaturalSimulation<T> {

        public:

            LinearCircuitSimulation(const size_t iNumComponents) :
                LinearNaturalSimulation<T>(iNumComponents) { ; }

            virtual size_t addComponent(std::unique_ptr<T> pComponent) {
                return LinearNaturalSimulation<T>::addComponent(std::move(pComponent));
            };

            void setStopTime(const double dStopTime) {
                DiscreteEventTimeDomainSimulation<T>::setStopTime(dStopTime);
            }

            double getTime() const {
                return DiscreteEventTimeDomainSimulation<T>::getTime();
            }

            void setTimeStep(const double dTimeStep) {
                DiscreteEventTimeDomainSimulation<T>::setTimeStep(dTimeStep);
            }

            double getVoltage(const size_t iNode) const {
                return LinearNaturalSimulation<T>::getAcross(iNode);
            }

            double getCurrent(const size_t iComponentIndex) const {
                return LinearNaturalSimulation<T>::getThrough(iComponentIndex);
            }

            virtual void initalize(bool bInitComponents) {
                LinearNaturalSimulation<T>::initalize(bInitComponents);
            }

            virtual bool step() {
                return LinearNaturalSimulation<T>::step();
            }
    };

    // C++ CLI needs this
    class LinearCircuitSimulationCC : public LinearCircuitSimulation<LinearCircuitSimComponent> {

        public:

            LinearCircuitSimulationCC(const size_t iNumComponents) :
                LinearCircuitSimulation<LinearCircuitSimComponent>(iNumComponents) { ; }

            virtual size_t addComponent(std::unique_ptr<LinearCircuitSimComponent> pComponent) {
                return LinearCircuitSimulation<LinearCircuitSimComponent>::addComponent(std::move(pComponent));
            };

            void setStopTime(const double dStopTime) {
                LinearCircuitSimulation<LinearCircuitSimComponent>::setStopTime(dStopTime);
            }

            double getTime() const {
                return LinearCircuitSimulation<LinearCircuitSimComponent>::getTime();
            }

            void setTimeStep(const double dTimeStep) {
                LinearCircuitSimulation<LinearCircuitSimComponent>::setTimeStep(dTimeStep);
            }

            double getVoltage(const size_t iNode) const {
                return LinearCircuitSimulation<LinearCircuitSimComponent>::getVoltage(iNode);
            }

            double getCurrent(const size_t iComponentIndex) const {
                return LinearCircuitSimulation<LinearCircuitSimComponent>::getCurrent(iComponentIndex);
            }

            virtual void initalize(bool bInitComponents) {
                LinearCircuitSimulation<LinearCircuitSimComponent>::initalize(bInitComponents);
            }

            virtual bool step() {
                return LinearCircuitSimulation<LinearCircuitSimComponent>::step();
            }
        };

}