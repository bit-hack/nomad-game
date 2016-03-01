file(GLOB HPP_FILES source/*.h)
file(GLOB CPP_FILES source/*.cpp)

add_library(nomad-human ${HPP_FILES} ${CPP_FILES})
target_link_libraries(nomad-human)
