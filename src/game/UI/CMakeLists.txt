
add_library(ui gamescreen.cpp)
# FTXUI Includes ---------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v4.1.1
)

FetchContent_GetProperties(ftxui)

if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------



target_link_libraries(ui
    PUBLIC ftxui::screen
    PUBLIC ftxui::dom
    PUBLIC ftxui::component
)

set_target_properties(ui PROPERTIES LINKER_LANGUAGE CXX)
