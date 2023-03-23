add_subdirectory(game)

add_library(source game)

set_target_properties(source PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(source PUBLIC game)