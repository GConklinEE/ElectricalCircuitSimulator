#pragma once

#include "Capacitor.h"
#include "CircuitComponent.h"
#include "GroundedVoltageSource.h"
#include "Inductor.h"
#include "LinearCircuit.h"
#include "Matrix.h"
#include "Resistor.h"

namespace SimulationEngineWrapper {

    template<class T>
    public ref class ManagedObject {

        public:

            ManagedObject(T* pInstance) :
                m_pInstance(pInstance)
            { ; }

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

            const T& getInstance() {
                return m_pInstance;
            }

        protected:

            T* m_pInstance;

    };
        
    public ref class PLU_Factorization : ManagedObject<SimulationEngine::PLU_Factorization> {

        public:

            PLU_Factorization() :
                ManagedObject(new SimulationEngine::PLU_Factorization()) { ; }
    };

    public ref class Matrix : ManagedObject<SimulationEngine::Matrix> {

        public:

            Matrix() :
                ManagedObject(new SimulationEngine::Matrix()) { ; }
            Matrix(const int iRows, const int iColumns) :
                ManagedObject(new SimulationEngine::Matrix(iRows, iColumns)) { ; }

            int getNumRows() { 
                return (int)(m_pInstance->getNumRows());
            }
            int getNumColumns() {
                return (int)(m_pInstance->getNumRows());
            }
            double getValue(const int iRow, const int iColumn) {
                return m_pInstance->getValue(iRow, iColumn);
            }
            void setValue(const int iRow, const int iColumn, const double dValue) {
                m_pInstance->setValue(iRow, iColumn, dValue);
            }
            void swapRows(const int iRow1, const int iRow2) {
                m_pInstance->swapRows(iRow1, iRow2);
            }
            void swapValues(const int iRow1, const int iColumn1, const int iRow2, const int iColumn2) {
                m_pInstance->swapValues(iRow1, iColumn1, iRow2, iColumn2);
            }
            void clear() {
                m_pInstance->clear();
            }
            void printMatrix() {
                m_pInstance->printMatrix();
            }
    };

    public ref class Capacitor : ManagedObject<SimulationEngine::Capacitor> {

        public:

            Capacitor(const int iNodeS, const int iNodeD, const double m_dCapacitance) :
                ManagedObject(new SimulationEngine::Capacitor(iNodeS, iNodeD, m_dCapacitance)) { ; }
    };

    public ref class CircuitComponent : ManagedObject<SimulationEngine::CircuitComponent> {

        public:

            CircuitComponent(const int iNodeS, const int iNodeD, bool bIsGround) :
                ManagedObject(new SimulationEngine::CircuitComponent(iNodeS, iNodeD, bIsGround)) { ; }

            bool getIsGround() {
                return m_pInstance->getIsGround();
            }
            int getNodeS() {
                return (int)(m_pInstance->getNodeS());
            }
            int getNodeD() {
                return (int)(m_pInstance->getNodeD());
            }
            double getCurrent() {
                return m_pInstance->getCurrent();
            }
    };

    public ref class GroundedVoltageSource : ManagedObject<SimulationEngine::GroundedVoltageSource> {
        
        public:

            GroundedVoltageSource(const int iNodeS, const int iNodeD, const double dVoltage, const double dResistance) :
                ManagedObject(new SimulationEngine::GroundedVoltageSource(iNodeS, iNodeD, dVoltage, dResistance)) { ; }
    };

    public ref class Inductor : ManagedObject<SimulationEngine::Inductor> {
        
        public:

            Inductor(const int iNodeS, const int iNodeD, const double m_dInductance) :
                ManagedObject(new SimulationEngine::Inductor(iNodeS, iNodeD, m_dInductance)) { ; }
    };
        
    public ref class LinearCircuit : ManagedObject<SimulationEngine::LinearCircuit> {

        public:

            LinearCircuit(const int iNumComponents) :
                ManagedObject(new SimulationEngine::LinearCircuit(iNumComponents)) { ; }

            int addResistor(const int iNodeS, const int iNodeD, const double dResistance) {
                return (int)(m_pInstance->addComponent(std::make_unique<SimulationEngine::Resistor>(iNodeS, iNodeD, dResistance)));
            }
            int addInductor(const int iNodeS, const int iNodeD, const double dInductance) {
                return (int)(m_pInstance->addComponent(std::make_unique<SimulationEngine::Inductor>(iNodeS, iNodeD, dInductance)));
            }
            int addCapacitor(const int iNodeS, const int iNodeD, const double dCapacitance) {
                return (int)(m_pInstance->addComponent(std::make_unique<SimulationEngine::Capacitor>(iNodeS, iNodeD, dCapacitance)));
            }
            int addGroundedVoltageSource(const int iNodeS, const int iNodeD, const double dVoltage, const double dResistance) {
                return (int)(m_pInstance->addComponent(std::make_unique<SimulationEngine::GroundedVoltageSource>(iNodeS, iNodeD, dVoltage, dResistance)));
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

    public ref class Resistor : ManagedObject<SimulationEngine::Resistor> {

        public:

            Resistor(const int iNodeS, const int iNodeD, const double dResistance) :
                ManagedObject(new SimulationEngine::Resistor(iNodeS, iNodeD, dResistance)) { ; }
    };
}
