add_subdirectory(systems)
add_subdirectory(components)
add_subdirectory(UI)

add_library(game
    entity.cpp
    )

set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(game 
PUBLIC ui
PUBLIC systems 
PUBLIC components
)
