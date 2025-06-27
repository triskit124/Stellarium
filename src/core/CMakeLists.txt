
set(STELL_CORE_SRCS
    Body.cpp
    Body.h
    Frame.h
    Integrators.cpp
    Integrators.h
    StellariumSimulation.cpp
    StellariumSimulation.h
)

target_sources(stellarium PRIVATE ${STELL_CORE_SRCS})
