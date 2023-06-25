

add_library(components
    aicomponent.cpp
    amuletslot.cpp
    armorcomponent.cpp
    armorslot.cpp
    buffcomponent.cpp
    coordinates.cpp
    critcomponent.cpp
    description.cpp
    effectcomponent.cpp
    experiencecomponent.cpp
    health.cpp
    hungercomponent.cpp
    inventory.cpp
    itemcomponent.cpp
    lineofsightcomponent.cpp
    name.cpp
    navmapcomponent.cpp
    tilecomponent.cpp
    weaponcomponent.cpp
    weaponslot.cpp
)

set_target_properties(components PROPERTIES LINKER_LANGUAGE CXX)
