#pragma once
#include "Exports.h"

extern "C"
{
  #pragma region Constructors and Destructors
  SIMULATION_ENGINE_EXPORTS void* createLinearCircuit(size_t numComponents);
  SIMULATION_ENGINE_EXPORTS void destroyLinearCircuit(void* linearCircuit);
  #pragma endregion

  #pragma region Modifiers
  SIMULATION_ENGINE_EXPORTS size_t addResistor(void* linearCircuit, size_t nodeS, size_t nodeD, double resistance);
  SIMULATION_ENGINE_EXPORTS size_t addInductor(void* linearCircuit, size_t nodeS, size_t nodeD, double inductance);
  SIMULATION_ENGINE_EXPORTS size_t addCapacitor(void* linearCircuit, size_t nodeS, size_t nodeD, double capacitance);
  SIMULATION_ENGINE_EXPORTS size_t addGroundedVoltageSource(void* linearCircuit, size_t nodeS, size_t nodeD, double voltage, double resistance);
  SIMULATION_ENGINE_EXPORTS void setStopTime(void* linearCircuit, double stopTime);
  SIMULATION_ENGINE_EXPORTS void setTimeStep(void* linearCircuit, double timeStep);
  SIMULATION_ENGINE_EXPORTS void initialize(void* linearCircuit);
  SIMULATION_ENGINE_EXPORTS bool step(void* linearCircuit);
  #pragma endregion

  #pragma region Observers
  SIMULATION_ENGINE_EXPORTS double getTime(void* linearCircuit);
  SIMULATION_ENGINE_EXPORTS double getVoltage(void* linearCircuit, size_t node);
  SIMULATION_ENGINE_EXPORTS double getCurrent(void* linearCircuit, size_t componentIndex);
  #pragma endregion
}