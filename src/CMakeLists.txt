add_subdirectory(game)
add_subdirectory(permissive_fov)

add_library(source game permissive_fov)

set_target_properties(source PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(source PUBLIC game PUBLIC permissive_fov)