

add_subdirectory(factories)
add_subdirectory(generators)

add_library(systems
    attacksystem.h
    actionsystem.h
    entitymanager.h
    mapmanager.h
    positionsystem.h
    healthsystem.h
    aisystem.h
)
target_link_libraries(systems
    PUBLIC factories
    PUBLIC generators)

set_target_properties(systems PROPERTIES LINKER_LANGUAGE CXX)