
set(STELL_CORE_SRCS
    Body.cpp
    Integrators.cpp
    StellariumSimulation.cpp
)

target_sources(stellarium PRIVATE ${STELL_CORE_SRCS})
