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

            NodeSimComponent(const size_t iNodeS, const size_t iNodeD);

            size_t getNodeS() const {
                return m_iNodeS;
            }
            size_t getNodeD() const {
                return m_iNodeD;
            }

        protected:

            size_t m_iNodeS; // Source
            size_t m_iNodeD; // Destination
    };

    class LinearNaturalSimComponent : public NodeSimComponent {

        public:

            LinearNaturalSimComponent(const size_t iNodeS, const size_t iNodeD, bool bHasAcrossReferenceNode);

            bool hasAcrossReferenceNode() const {
                return m_bHasAcrossReferenceNode;
            }
            double getThrough() const { // Through param going through component
                return m_dThrough;
            }
            virtual void LNS_initalize(Matrix<double>& oSimulationMatrix, const double dTimeStep);
            virtual void LNS_step(Matrix<double>& oThroughVector);
            virtual void LNS_postStep(Matrix<double>& oAcrossVector);

        protected:

            bool m_bHasAcrossReferenceNode;
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

            LinearCircuitSimComponent(const size_t iNodeS, const size_t iNodeD, bool bHasGroundNode) :
                LinearNaturalSimComponent(iNodeS, iNodeD, bHasGroundNode) { ; }

            bool hasGroundNode() const {
                return hasAcrossReferenceNode();
            }
            double getCurrent() const { // Current going through component
                return getThrough();
            }
    };

}