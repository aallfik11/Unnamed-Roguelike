

add_library(components
    coordinates.h
    description.h
    health.h
    inventory.h
    name.h
    stackable.h
    tilecomponent.h
    interaction.h
    aicomponent.h
    navmapcomponent.h
    lineofsightcomponent.h
    weaponcomponent.h
    typecomponent.h
    armorcomponent.h
    experiencecomponent.h
    hungercomponent.h
    itemcomponent.h
    buffcomponent.h
    amuletslot.h
    weaponslot.h
    armorslot.h 
)

set_target_properties(components PROPERTIES LINKER_LANGUAGE CXX)