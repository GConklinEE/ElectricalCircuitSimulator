#pragma once

#include "Matrix.h"

namespace SimulationEngine {

    class DiscreteEventTimeDomainSimComponent {

        public:

            DiscreteEventTimeDomainSimComponent() { ; }
       
            virtual void DETDS_initalize(const double dTimeStep);
            virtual void DETDS_step();
    };

    class NodeSimComponent : public DiscreteEventTimeDomainSimComponent {

        public:

            NodeSimComponent(const size_t iNodeList);

            size_t getNumNodes() const;
            size_t getNode(const size_t iNodeIndex) const;
            void setNodeList(const size_t iNodeList) {
                m_iNodeList = iNodeList;
            };

        protected:

            size_t BITS_PER_NODE = 4;
            size_t NODE_BITMASK = 0xF;
            size_t MAX_NODES = 16; // 64 bits / BITS_PER_NODE 
            size_t m_iNodeList;
    };

    class LinearNaturalSimComponent : public NodeSimComponent {

        public:

            LinearNaturalSimComponent(const size_t iNodeList, const bool bHasAcrossReferenceNode, const size_t iAcrossReferenceNode);

            bool hasAcrossReferenceNode() const {
                return m_bHasAcrossReferenceNode;
            }
            size_t getAcrossReferenceNode() const {
                return m_iAcrossReferenceNode;
            }
            double getThrough() const { // Through param going through component
                return m_dThrough;
            }
            virtual void LNS_initalize(Matrix<double>& oSimulationMatrix, const double dTimeStep);
            virtual void LNS_step(Matrix<double>& oThroughVector);
            virtual void LNS_postStep(Matrix<double>& oAcrossVector);

        protected:

            bool m_bHasAcrossReferenceNode;
            size_t m_iAcrossReferenceNode;
            double m_dComponentSimulationMatrixStamp;
            double m_dThrough; // Through param is positive if flowing from source to destination, negative if the opposite direction

            virtual void applySimulationMatrixStamp(Matrix<double>& oSimulationMatrix, const double dTimeStep);
            virtual void applyThroughVectorMatrixStamp(Matrix<double>& oThroughVector);
    };

    // AcrossReferenceNode = Circuit Ground
    // ComponentSimulationMatrixStamp = Component Resistance Matrix Stamp
    // applyThroughVectorMatrixStamp = Component Current Vector Stamp
    // Across = Voltage (V)
    // Through = Current (A)
    class LinearCircuitSimComponent : public LinearNaturalSimComponent {

        public:

            LinearCircuitSimComponent(const size_t iNodeList, const bool bHasGroundNode, const size_t iGroundNode) :
                LinearNaturalSimComponent(iNodeList, bHasGroundNode, iGroundNode) { ; }

            bool hasGroundNode() const {
                return hasAcrossReferenceNode();
            }
            bool getGroundNode() const {
                return getAcrossReferenceNode();
            }
            double getCurrent() const { // Current going through component
                return getThrough();
            }
    };

}