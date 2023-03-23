
add_subdirectory(systems)
add_subdirectory(components)

add_library(game
    component.h
    coordhash.h
    directions.h
    entity.h
    tile.h
    )

set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(game 
PUBLIC systems 
PUBLIC components
)