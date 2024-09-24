// SimulationEngineDebugger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Capacitor.h"
#include "Component.h"
#include "GroundedVoltageSource.h"
#include "Inductor.h"
#include "Simulation.h"
#include "Matrix.h"
#include "Resistor.h"

using namespace SimulationEngine;
using std::cout;
using std::endl;
using std::make_unique;

void SimulationIntegrationTestSeriesRR()
{
    bool bDone;
    size_t iSteps = 0;
    LinearCircuitSimulation<LinearCircuitSimComponent> oLinearCircuit = LinearCircuitSimulation<LinearCircuitSimComponent>(3);

    oLinearCircuit.addComponent(make_unique<GroundedVoltageSource>(2, 1, 30, 10)); // Node 2 is ground
    oLinearCircuit.addComponent(make_unique<Resistor>(1, 0, 10));
    oLinearCircuit.addComponent(make_unique<Resistor>(0, 2, 10));
    oLinearCircuit.setStopTime(10);
    oLinearCircuit.setTimeStep(1);
    oLinearCircuit.initalize(true);

    do
    {
        bDone = oLinearCircuit.step();
        iSteps++;
    } while (bDone == false && iSteps < 20);

    cout << "\n********************* Results *********************" << endl;
    cout << "Steps run (expect 10): " << iSteps << endl;
    cout << "Time at end of sim (expect 10): " << oLinearCircuit.getTime() << endl;
    cout << "Voltage at node 0 (expect 10): " << oLinearCircuit.getVoltage(0) << endl;
    cout << "Voltage at node 1 (expect 20): " << oLinearCircuit.getVoltage(1) << endl;
    cout << "Voltage at node 2 (expect 0): " << oLinearCircuit.getVoltage(2) << endl;
    cout << "Current through component 0 (expect 1): " << oLinearCircuit.getCurrent(0) << endl;
    cout << "Current through component 1 (expect 1): " << oLinearCircuit.getCurrent(1) << endl;
    cout << "Current through component 2 (expect 1): " << oLinearCircuit.getCurrent(2) << endl;
}

void SimulationIntegrationTestSeriesRC()
{
    bool bDone;
    size_t iSteps = 0;
    LinearCircuitSimulation<LinearCircuitSimComponent> oLinearCircuit = LinearCircuitSimulation<LinearCircuitSimComponent>(3);

    oLinearCircuit.addComponent(make_unique<GroundedVoltageSource>(2, 1, 30, 10)); // Node 2 is ground
    oLinearCircuit.addComponent(make_unique<Resistor>(1, 0, 10));
    oLinearCircuit.addComponent(make_unique<Capacitor>(0, 2, 0.2));
    oLinearCircuit.setStopTime(10);
    oLinearCircuit.setTimeStep(1);
    oLinearCircuit.initalize(true);

    do
    {
        bDone = oLinearCircuit.step();
        iSteps++;
    } while (bDone == false && iSteps < 20);

    cout << "\n********************* Results *********************" << endl;
    cout << "Steps run (expect 10): " << iSteps << endl;
    cout << "Time at end of sim (expect 10): " << oLinearCircuit.getTime() << endl;
    cout << "Voltage at node 0 (expect 27.2224): " << oLinearCircuit.getVoltage(0) << endl;
    cout << "Voltage at node 1 (expect 28.6112): " << oLinearCircuit.getVoltage(1) << endl;
    cout << "Voltage at node 2 (expect 0): " << oLinearCircuit.getVoltage(2) << endl;
    cout << "Current through component 0 (expect 0.13888): " << oLinearCircuit.getCurrent(0) << endl;
    cout << "Current through component 1 (expect 0.13888): " << oLinearCircuit.getCurrent(1) << endl;
    cout << "Current through component 2 (expect 0.13888): " << oLinearCircuit.getCurrent(2) << endl;
}

void SimulationIntegrationTestSeriesRL()
{
    bool bDone;
    size_t iSteps = 0;
    LinearCircuitSimulation<LinearCircuitSimComponent> oLinearCircuit = LinearCircuitSimulation<LinearCircuitSimComponent>(3);

    oLinearCircuit.addComponent(make_unique<GroundedVoltageSource>(2, 1, 30, 10)); // Node 2 is ground
    oLinearCircuit.addComponent(make_unique<Resistor>(1, 0, 10));
    oLinearCircuit.addComponent(make_unique<Inductor>(0, 2, 50));
    oLinearCircuit.setStopTime(10);
    oLinearCircuit.setTimeStep(1);
    oLinearCircuit.initalize(true);

    do
    {
        bDone = oLinearCircuit.step();
        iSteps++;
    } while (bDone == false && iSteps < 20);

    cout << "\n********************* Results *********************" << endl;
    cout << "Steps run (expect 10): " << iSteps << endl;
    cout << "Time at end of sim (expect 10): " << oLinearCircuit.getTime() << endl;
    cout << "Voltage at node 0 (expect 0.650307): " << oLinearCircuit.getVoltage(0) << endl;
    cout << "Voltage at node 1 (expect 15.3252): " << oLinearCircuit.getVoltage(1) << endl;
    cout << "Voltage at node 2 (expect 0): " << oLinearCircuit.getVoltage(2) << endl;
    cout << "Current through component 0 (expect 1.46748): " << oLinearCircuit.getCurrent(0) << endl;
    cout << "Current through component 1 (expect 1.46748): " << oLinearCircuit.getCurrent(1) << endl;
    cout << "Current through component 2 (expect 1.46748): " << oLinearCircuit.getCurrent(2) << endl;
}

int main()
{
    cout << "********************* Simulation Engine Debugger *********************\n\n" << endl;
    // Enable whatever circuit system you want to debug
    SimulationIntegrationTestSeriesRR();
    //SimulationIntegrationTestSeriesRC();
    //SimulationIntegrationTestSeriesRL();
}