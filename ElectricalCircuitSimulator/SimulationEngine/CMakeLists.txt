file(GLOB_RECURSE SIMULATION_ENGINE_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE SIMULATION_ENGINE_SOURCE src/*.cpp)

add_library(SimulationEngine STATIC ${SIMULATION_ENGINE_HEADERS} ${SIMULATION_ENGINE_SOURCE})
target_include_directories(SimulationEngine PUBLIC include)