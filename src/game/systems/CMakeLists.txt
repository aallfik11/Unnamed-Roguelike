add_subdirectory(factories)
add_subdirectory(generators)

find_library(components components)

add_library(systems
    aisystem.cpp
    attacksystem.cpp
    effectsystem.cpp
    entitymanager.cpp
    experiencesystem.cpp
    healthsystem.cpp
    inventorysystem.cpp
    lineofsightsystem.cpp
    logsystem.cpp
    mapmanager.cpp
    navmapmanager.cpp
    playersystem.cpp
    positionsystem.cpp
)
target_link_libraries(systems
    PRIVATE components 
    PUBLIC factories
    PUBLIC generators)

set_target_properties(systems PROPERTIES LINKER_LANGUAGE CXX)
