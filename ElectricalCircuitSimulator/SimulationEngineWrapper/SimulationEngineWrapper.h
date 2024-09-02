#pragma once

#include "../SimulationEngine/Capacitor.h"
#include "../SimulationEngine/CircuitComponent.h"
#include "../SimulationEngine/GroundedVoltageSource.h"
#include "../SimulationEngine/Inductor.h"
#include "../SimulationEngine/LinearCircuit.h"
#include "../SimulationEngine/Matrix.h"
#include "../SimulationEngine/Resistor.h"

using namespace System;
using namespace SimulationEngine;

namespace SimulationEngineWrapper {

    template<class T>
    public ref class ManagedObject {

        public:

            ManagedObject(T* pInstance) : m_pInstance(pInstance) {
                ;
            }

            virtual ~ManagedObject() {
                if (m_pInstance != nullptr) {
                    delete m_pInstance;
                }
            }
            !ManagedObject() {
                if (m_pInstance != nullptr) {
                    delete m_pInstance;
                }
            }

            T* getInstance() {
                return m_pInstance;
            }

        protected:

            T* m_pInstance;

    };
        
    public ref class PLU_Factorization : public ManagedObject<SimulationEngine::PLU_Factorization> {

		public:

			PLU_Factorization() 
            : ManagedObject(new SimulationEngine::PLU_Factorization()) { ; }
	};

    public ref class Matrix : public ManagedObject<SimulationEngine::Matrix> {

		public:

			Matrix()
            : ManagedObject(new SimulationEngine::Matrix()) { ; }
			Matrix(const int iRows, const int iColumns)
            : ManagedObject(new SimulationEngine::Matrix(iRows, iColumns)) { ; }

			int getNumRows() { 
                return m_pInstance->getNumRows();
			}
			int getNumColumns() {
                return m_pInstance->getNumRows();
			}
            double getValue(const int iRow, const int iColumn) {
                return m_pInstance->getValue(iRow, iColumn);
            }
            void setValue(const int iRow, const int iColumn, const double dValue) {
                m_pInstance->setValue(iRow, iColumn, dValue);
            }
            void clear() {
                m_pInstance->clear();
            }
            void clone() {
                SimulationEngine::Matrix* pClone;
                pClone = m_pInstance->clone();
                delete pClone;
            }
            void printMatrix() {
                m_pInstance->printMatrix();
            }
	};

    public ref class Capacitor : public ManagedObject<SimulationEngine::Capacitor> {

        public:

            Capacitor(const int iNodeS, const int iNodeD, const double m_dCapacitance)
            : ManagedObject(new SimulationEngine::Capacitor(iNodeS, iNodeD, m_dCapacitance)) { ; }
    };

    public ref class CircuitComponent : public ManagedObject<SimulationEngine::CircuitComponent> {

		public:

			CircuitComponent(const int iNodeS, const int iNodeD, bool bIsGround)
            : ManagedObject(new SimulationEngine::CircuitComponent(iNodeS, iNodeD, bIsGround)) { ; }

			bool getIsGround() {
                return m_pInstance->getIsGround();
			}
			int getNodeS() {
				return m_pInstance->getNodeS();
			}
			int getNodeD() {
				return m_pInstance->getNodeD();
			}
			double getCurrent() {
				return m_pInstance->getCurrent();
			}
	};

    public ref class GroundedVoltageSource : public ManagedObject<SimulationEngine::GroundedVoltageSource> {
        
        public:

            GroundedVoltageSource(const int iNodeS, const int iNodeD, const double dVoltage, const double dResistance)
            : ManagedObject(new SimulationEngine::GroundedVoltageSource(iNodeS, iNodeD, dVoltage, dResistance)) { ; }
    };

    public ref class Inductor : public ManagedObject<SimulationEngine::Inductor> {
        
        public:

            Inductor(const int iNodeS, const int iNodeD, const double m_dInductance)
            : ManagedObject(new SimulationEngine::Inductor(iNodeS, iNodeD, m_dInductance)) { ; }
    };
        
    public ref class LinearCircuit : public ManagedObject<SimulationEngine::LinearCircuit> {

        public:

            LinearCircuit(const int iNumComponents)
            : ManagedObject(new SimulationEngine::LinearCircuit(iNumComponents)) { ; }

            int addResistor(const int iNodeS, const int iNodeD, const double dResistance) {
                SimulationEngine::Resistor* pResistor = new SimulationEngine::Resistor(iNodeS, iNodeD, dResistance);
                return m_pInstance->addComponent(pResistor);
            }
            int addInductor(const int iNodeS, const int iNodeD, const double dInductance) {
                SimulationEngine::Inductor* pInductor = new SimulationEngine::Inductor(iNodeS, iNodeD, dInductance);
                return m_pInstance->addComponent(pInductor);
            }
            int addCapacitor(const int iNodeS, const int iNodeD, const double dCapacitance) {
                SimulationEngine::Capacitor* pCapacitor = new::Capacitor(iNodeS, iNodeD, dCapacitance);
                return m_pInstance->addComponent(pCapacitor);
            }
            int addGroundedVoltageSource(const int iNodeS, const int iNodeD, const double dVoltage, const double dResistance) {
                SimulationEngine::GroundedVoltageSource* pGroundedVoltageSource = new SimulationEngine::GroundedVoltageSource(iNodeS, iNodeD, dVoltage, dResistance);
                return m_pInstance->addComponent(pGroundedVoltageSource);
            }
            void setStopTime(const double dStopTime) {
                m_pInstance->setStopTime(dStopTime);
            }
            void setTimeStep(const double dTimeStep) {
                m_pInstance->setTimeStep(dTimeStep);
            }
            double getTime() {
                return m_pInstance->getTime();
            }
            double getVoltage(const int iNode) {
                return m_pInstance->getVoltage(iNode);
            }
            double getCurrent(const int iComponentIndex) {
                return m_pInstance->getCurrent(iComponentIndex);
            }
            void initalize() {
                m_pInstance->initalize();
            }
            bool step() {
                return m_pInstance->step();
            }
    };

    public ref class Resistor : public ManagedObject<SimulationEngine::Resistor> {

        public:

            Resistor(const int iNodeS, const int iNodeD, const double dResistance)
            : ManagedObject(new SimulationEngine::Resistor(iNodeS, iNodeD, dResistance)) { ; }
    };
}
