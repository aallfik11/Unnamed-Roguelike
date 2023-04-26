
add_subdirectory(systems)
add_subdirectory(components)

add_library(game
    action.h
    ai_enum.h
    component.h
    coordhash.h
    directions.h
    effect.h
    entity.h
    health_enum.h
    tile.h
    globals.h
    system.h
    entityholder.h
    )

set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(game 
PUBLIC systems 
PUBLIC components
)
