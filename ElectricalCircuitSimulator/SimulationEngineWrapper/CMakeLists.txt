file(GLOB_RECURSE SIMULATION_ENGINE_WRAPPER_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE SIMULATION_ENGINE_WRAPPER_SOURCE src/*.cpp)

add_library(SimulationEngineWrapper SHARED ${SIMULATION_ENGINE_WRAPPER_HEADERS} ${SIMULATION_ENGINE_WRAPPER_SOURCE})
target_include_directories(SimulationEngineWrapper PUBLIC include)
target_link_libraries(SimulationEngineWrapper PUBLIC SimulationEngine PRIVATE "System.dll")
set_target_properties(SimulationEngineWrapper PROPERTIES COMMON_LANGUAGE_RUNTIME "")
target_compile_options(SimulationEngineWrapper PRIVATE "/clr" "/RTC1-")