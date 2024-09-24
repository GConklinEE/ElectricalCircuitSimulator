#include "LinearCircuit.hpp"
#include "LinearCircuit.h"
#include "Resistor.h"
#include "Inductor.h"
#include "Capacitor.h"
#include "GroundedVoltageSource.h"

using namespace SimulationEngine;
using namespace std;

#pragma region Constructors and Destructors
void* createLinearCircuit(size_t numComponents)
{
  return new LinearCircuit(numComponents);
}

void destroyLinearCircuit(void* linearCircuit)
{
  delete reinterpret_cast<LinearCircuit*>(linearCircuit);
}
#pragma endregion

#pragma region Modifiers
size_t addResistor(void* linearCircuit, size_t nodeS, size_t nodeD, double resistance)
{
  return reinterpret_cast<LinearCircuit*>(linearCircuit)->addComponent(make_unique<Resistor>(nodeS, nodeD, resistance));
}

size_t addInductor(void* linearCircuit, size_t nodeS, size_t nodeD, double inductance)
{
  return reinterpret_cast<LinearCircuit*>(linearCircuit)->addComponent(make_unique<Inductor>(nodeS, nodeD, inductance));
}

size_t addCapacitor(void* linearCircuit, size_t nodeS, size_t nodeD, double capacitance)
{
  return reinterpret_cast<LinearCircuit*>(linearCircuit)->addComponent(make_unique<Capacitor>(nodeS, nodeD, capacitance));
}

size_t addGroundedVoltageSource(void* linearCircuit, size_t nodeS, size_t nodeD, double voltage, double resistance)
{
  return reinterpret_cast<LinearCircuit*>(linearCircuit)->addComponent(make_unique<GroundedVoltageSource>(nodeS, nodeD, voltage, resistance));
}

void setStopTime(void* linearCircuit, double stopTime)
{
  reinterpret_cast<LinearCircuit*>(linearCircuit)->setStopTime(stopTime);
}

void setTimeStep(void* linearCircuit, double timeStep)
{
  reinterpret_cast<LinearCircuit*>(linearCircuit)->setTimeStep(timeStep);
}

void initialize(void* linearCircuit)
{
  reinterpret_cast<LinearCircuit*>(linearCircuit)->initalize();
}

bool step(void* linearCircuit)
{
  return reinterpret_cast<LinearCircuit*>(linearCircuit)->step();
}
#pragma endregion

#pragma region Observers
double getTime(void* linearCircuit)
{
  return reinterpret_cast<LinearCircuit*>(linearCircuit)->getTime();
}

double getVoltage(void* linearCircuit, size_t node)
{
  return reinterpret_cast<LinearCircuit*>(linearCircuit)->getVoltage(node);
}

double getCurrent(void* linearCircuit, size_t componentIndex)
{
  return reinterpret_cast<LinearCircuit*>(linearCircuit)->getCurrent(componentIndex);
}
#pragma endregion