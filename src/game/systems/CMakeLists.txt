

add_subdirectory(factories)
add_subdirectory(generators)

add_library(systems
    entitymanager.h
    mapmanager.h
    positionsystem.h
    healthsystem.h
    aisystem.h
    navmapmanager.h
    lineofsightsystem.h
    inventorysystem.h
    attacksystem.h
    effectsystem.h
    playerinputsystem.h
)
target_link_libraries(systems
    PUBLIC factories
    PUBLIC generators)

set_target_properties(systems PROPERTIES LINKER_LANGUAGE CXX)
