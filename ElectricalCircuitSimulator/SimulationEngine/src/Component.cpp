#include "Component.h"
#include <iostream>

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

    NodeSimComponent::NodeSimComponent(const size_t iNodeS, const size_t iNodeD) :
        m_iNodeS(iNodeS),
        m_iNodeD(iNodeD)
    {
        if (iNodeS == iNodeD) {
            cout << "Node values must not be the same!" << endl;
            throw invalid_argument("Node values must not be the same!");
        }
    }

    // If the component has an across reference node, iNodeS is assumed to be that node
    LinearNaturalSimComponent::LinearNaturalSimComponent(const size_t iNodeS, const size_t iNodeD, bool bHasAcrossReferenceNode) :
        NodeSimComponent::NodeSimComponent(iNodeS, iNodeD),
        m_bHasAcrossReferenceNode(bHasAcrossReferenceNode),
        m_dComponentSimulationMatrixStamp(0.0),
        m_dThrough(0.0) { ; }

    void DiscreteEventTimeDomainSimComponent::DETDS_initalize(const double dTimeStep) {
        ;
    }

    void LinearNaturalSimComponent::LNS_initalize(Matrix<double>& oSimulationMatrix, const double dTimeStep) {
        ;
    }

    void DiscreteEventTimeDomainSimComponent::DETDS_step() {
        ;
    }

    void LinearNaturalSimComponent::LNS_step(Matrix<double>& oThroughVector) {
        ;
    }

    void LinearNaturalSimComponent::LNS_postStep(Matrix<double>& oAcrossVector) {
        ;
    }

    void LinearNaturalSimComponent::applySimulationMatrixStamp(Matrix<double>& oConoSimulationMatrixductanceMatrix, const double dTimeStep) {
        ;
    }

    void LinearNaturalSimComponent::applyThroughVectorMatrixStamp(Matrix<double>& oThroughVector) {
        ;
    }
}