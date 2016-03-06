file(GLOB HPP_FILES source/*.h)
file(GLOB CPP_FILES source/*.cpp)

add_library(nomad-objects ${HPP_FILES} ${CPP_FILES})
target_link_libraries(nomad-objects)
