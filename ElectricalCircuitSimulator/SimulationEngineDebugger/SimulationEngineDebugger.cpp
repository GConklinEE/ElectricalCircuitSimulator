// SimulationEngineDebugger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Capacitor.h"
#include "CircuitComponent.h"
#include "GroundedVoltageSource.h"
#include "Inductor.h"
#include "LinearCircuit.h"
#include "Matrix.h"
#include "Resistor.h"

using namespace SimulationEngine;
using std::cout;
using std::endl;

void SimulationIntegrationTestSeriesRR()
{
    bool bDone;
    int iSteps = 0;
    LinearCircuit* pLinearCircuit = new LinearCircuit(3);
    GroundedVoltageSource* pGroundedVoltageSource = new GroundedVoltageSource(2, 1, 30, 10); // Node 2 is ground
    Resistor* pResistor1 = new Resistor(1, 0, 10);
    Resistor* pResistor2 = new Resistor(0, 2, 10);

    pLinearCircuit->addComponent(pGroundedVoltageSource);
    pLinearCircuit->addComponent(pResistor1);
    pLinearCircuit->addComponent(pResistor2);
    pLinearCircuit->setStopTime(10);
    pLinearCircuit->setTimeStep(1);
    pLinearCircuit->initalize();

    do
    {
        bDone = pLinearCircuit->step();
        iSteps++;
    } while (bDone == false && iSteps < 20);

    cout << "\n********************* Results *********************" << endl;
    cout << "Steps run (expect 10): " << iSteps << endl;
    cout << "Time at end of sim (expect 10): " << pLinearCircuit->getTime() << endl;
    cout << "Voltage at node 0 (expect 10): " << pLinearCircuit->getVoltage(0) << endl;
    cout << "Voltage at node 1 (expect 20): " << pLinearCircuit->getVoltage(1) << endl;
    cout << "Voltage at node 2 (expect 0): " << pLinearCircuit->getVoltage(2) << endl;
    cout << "Current through component 0 (expect 1): " << pLinearCircuit->getCurrent(0) << endl;
    cout << "Current through component 1 (expect 1): " << pLinearCircuit->getCurrent(1) << endl;
    cout << "Current through component 2 (expect 1): " << pLinearCircuit->getCurrent(2) << endl;

    delete pLinearCircuit;
    delete pGroundedVoltageSource;
    delete pResistor1;
    delete pResistor2;
}

void SimulationIntegrationTestSeriesRC()
{
    bool bDone;
    int iSteps = 0;
    LinearCircuit* pLinearCircuit = new LinearCircuit(3);
    GroundedVoltageSource* pGroundedVoltageSource = new GroundedVoltageSource(2, 1, 30, 10); // Node 2 is ground
    Resistor* pResistor = new Resistor(1, 0, 10);
    Capacitor* pCapacitor = new Capacitor(0, 2, 0.2);

    pLinearCircuit->addComponent(pGroundedVoltageSource);
    pLinearCircuit->addComponent(pResistor);
    pLinearCircuit->addComponent(pCapacitor);
    pLinearCircuit->setStopTime(10);
    pLinearCircuit->setTimeStep(1);
    pLinearCircuit->initalize();

    do
    {
        bDone = pLinearCircuit->step();
        iSteps++;
    } while (bDone == false && iSteps < 20);

    cout << "\n********************* Results *********************" << endl;
    cout << "Steps run (expect 10): " << iSteps << endl;
    cout << "Time at end of sim (expect 10): " << pLinearCircuit->getTime() << endl;
    cout << "Voltage at node 0 (expect 27.2224): " << pLinearCircuit->getVoltage(0) << endl;
    cout << "Voltage at node 1 (expect 28.6112): " << pLinearCircuit->getVoltage(1) << endl;
    cout << "Voltage at node 2 (expect 0): " << pLinearCircuit->getVoltage(2) << endl;
    cout << "Current through component 0 (expect 0.13888): " << pLinearCircuit->getCurrent(0) << endl;
    cout << "Current through component 1 (expect 0.13888): " << pLinearCircuit->getCurrent(1) << endl;
    cout << "Current through component 2 (expect 0.13888): " << pLinearCircuit->getCurrent(2) << endl;

    delete pLinearCircuit;
    delete pGroundedVoltageSource;
    delete pResistor;
    delete pCapacitor;
}

void SimulationIntegrationTestSeriesRL()
{
    bool bDone;
    int iSteps = 0;
    LinearCircuit* pLinearCircuit = new LinearCircuit(3);
    GroundedVoltageSource* pGroundedVoltageSource = new GroundedVoltageSource(2, 1, 30, 10); // Node 2 is ground
    Resistor* pResistor = new Resistor(1, 0, 10);
    Inductor* pInductor = new Inductor(0, 2, 50);

    pLinearCircuit->addComponent(pGroundedVoltageSource);
    pLinearCircuit->addComponent(pResistor);
    pLinearCircuit->addComponent(pInductor);
    pLinearCircuit->setStopTime(10);
    pLinearCircuit->setTimeStep(1);
    pLinearCircuit->initalize();

    do
    {
        bDone = pLinearCircuit->step();
        iSteps++;
    } while (bDone == false && iSteps < 20);

    cout << "\n********************* Results *********************" << endl;
    cout << "Steps run (expect 10): " << iSteps << endl;
    cout << "Time at end of sim (expect 10): " << pLinearCircuit->getTime() << endl;
    cout << "Voltage at node 0 (expect 0.650307): " << pLinearCircuit->getVoltage(0) << endl;
    cout << "Voltage at node 1 (expect 15.3252): " << pLinearCircuit->getVoltage(1) << endl;
    cout << "Voltage at node 2 (expect 0): " << pLinearCircuit->getVoltage(2) << endl;
    cout << "Current through component 0 (expect 1.46748): " << pLinearCircuit->getCurrent(0) << endl;
    cout << "Current through component 1 (expect 1.46748): " << pLinearCircuit->getCurrent(1) << endl;
    cout << "Current through component 2 (expect 1.46748): " << pLinearCircuit->getCurrent(2) << endl;

    delete pLinearCircuit;
    delete pGroundedVoltageSource;
    delete pResistor;
    delete pInductor;
}

int main()
{
    cout << "********************* Simulation Engine Debugger *********************\n\n" << endl;
    // Enable whatever circuit system you want to debug
    SimulationIntegrationTestSeriesRR();
    //SimulationIntegrationTestSeriesRC();
    //SimulationIntegrationTestSeriesRL();
}