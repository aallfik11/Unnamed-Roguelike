cmake_minimum_required(VERSION 3.0.0)
project(Unnamed-Roguelike LANGUAGES CXX VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# FTXUI Includes ---------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v4.0.0
)

FetchContent_GetProperties(ftxui)

if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------
include(CTest)
enable_testing()

set(GAMEHEADERS src/game)
# set(SOURCES test.cpp ${GAMEHEADERS}/armor.h ${GAMEHEADERS}/consumable.h ${GAMEHEADERS}/entity_animate.h ${GAMEHEADERS}/entity.h ${GAMEHEADERS}/item.h ${GAMEHEADERS}/monster.h ${GAMEHEADERS}/player.h ${GAMEHEADERS}/rarity.h ${GAMEHEADERS}/tile.h ${GAMEHEADERS}/TileTypes.h ${GAMEHEADERS}/weapon.h)
# set(SOURCES tester.cpp ${GAMEHEADERS}/entity.h ${GAMEHEADERS}/component.h)

add_executable(Unnamed-Roguelike tester.cpp)

target_include_directories(Unnamed-Roguelike PRIVATE ${GAMEHEADERS})

target_link_libraries(Unnamed-Roguelike
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)




set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
