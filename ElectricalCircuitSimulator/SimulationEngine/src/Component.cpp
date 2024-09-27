#include "Component.h"
#include <iostream>

using std::cout;
using std::endl;
using std::invalid_argument;

namespace SimulationEngine {

    NodeSimComponent::NodeSimComponent(const size_t iNodeList) :
        m_iNodeList(iNodeList)
    {
        size_t iNodeIndex1;
        size_t iNodeIndex2;
        size_t iNumNodes = getNumNodes();

        for (iNodeIndex1 = 0; iNodeIndex1 < iNumNodes; iNodeIndex1++) {
            for (iNodeIndex2 = iNodeIndex1 + 1; iNodeIndex2 < iNumNodes; iNodeIndex2++) {
                if (getNode(iNodeIndex1) == getNode(iNodeIndex2)) {
                    cout << "Two node values must not be the same!" << endl;
                    throw invalid_argument("Two node values must not be the same!");
                }
            }
        }
    }

    // If the component has an across reference node, iNodeS is assumed to be that node
    LinearNaturalSimComponent::LinearNaturalSimComponent(const size_t iNodeList, const bool bHasAcrossReferenceNode, const size_t iAcrossReferenceNode) :
        NodeSimComponent::NodeSimComponent(iNodeList),
        m_bHasAcrossReferenceNode(bHasAcrossReferenceNode),
        m_iAcrossReferenceNode(iAcrossReferenceNode),
        m_dComponentSimulationMatrixStamp(0.0),
        m_dThrough(0.0) { ; }

    size_t NodeSimComponent::getNumNodes() const {
        size_t iNumNodes = 1; // The zero node always exists somewhere
        size_t iNode;

        for (iNode = 0; iNode < MAX_NODES; iNode++) {
            if (((m_iNodeList >> (BITS_PER_NODE * iNode)) & NODE_BITMASK) != 0) {
                iNumNodes++;
            }
        }

        return iNumNodes;
    }

    size_t NodeSimComponent::getNode(const size_t iNodeIndex) const {
        if ((iNodeIndex >= getNumNodes()) || (iNodeIndex >= MAX_NODES)) {
            cout << "Index is out of bounds!" << endl;
            throw invalid_argument("Index is out of bounds!");
        }

        return (m_iNodeList >> (BITS_PER_NODE * iNodeIndex)) & NODE_BITMASK;
    }

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