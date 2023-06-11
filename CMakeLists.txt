cmake_minimum_required(VERSION 3.26)
project(Unnamed-Roguelike LANGUAGES CXX VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_FLAGS "-Wall -Wextra -static-libstdc++ -static-libgcc")
# set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize=undefined -Wall -Wextra")
# set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize=undefined")
# set(CMAKE_CXX_FLAGS "-fsanitize=thread")
# set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


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
include(CTest)
enable_testing()

# set(GAMEHEADERS src/game)
add_subdirectory(src)

add_executable(Unnamed-Roguelike demo.cpp)

# target_include_directories(Unnamed-Roguelike PRIVATE ${GAMEHEADERS})
target_link_libraries(Unnamed-Roguelike PUBLIC source)

target_link_libraries(Unnamed-Roguelike
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Unnamed-Roguelike")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_NAME "Unnamed Roguelike")
install(TARGETS Unnamed-Roguelike RUNTIME DESTINATION bin)
install(TARGETS source DESTINATION include)
include(CPack)
