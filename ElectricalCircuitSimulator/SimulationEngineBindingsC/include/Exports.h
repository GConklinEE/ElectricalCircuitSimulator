#pragma once

#ifdef _WIN32
  #ifdef SIMULATION_ENGINE_BINDINGS
    #define SIMULATION_ENGINE_EXPORTS _declspec(dllexport)
  #else
    #define SIMULATION_ENGINE_EXPORTS __declspec(dllimport)
  #endif
#else
  #define SIMULATION_ENGINE_EXPORTS
#endif
