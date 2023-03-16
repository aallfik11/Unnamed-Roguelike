cmake_minimum_required(VERSION 3.0.0)
project(Unnamed-Roguelike LANGUAGES CXX VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

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

set(SOURCEFOLDER src)
set(SOURCES main.cpp ${SOURCEFOLDER}/armor.h ${SOURCEFOLDER}/consumable.h ${SOURCEFOLDER}/entity_animate.h ${SOURCEFOLDER}/entity.h ${SOURCEFOLDER}/item.h ${SOURCEFOLDER}/monster.h ${SOURCEFOLDER}/player.h ${SOURCEFOLDER}/rarity.h ${SOURCEFOLDER}/tile.h ${SOURCEFOLDER}/TileTypes.h ${SOURCEFOLDER}/weapon.h)


add_executable(Unnamed-Roguelike ${SOURCES})

target_include_directories(Unnamed-Roguelike PRIVATE ${SOURCEFOLDER})

target_link_libraries(Unnamed-Roguelike
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)




set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
