file(GLOB HPP_FILES source/*.h)
file(GLOB CPP_FILES source/*.cpp)

add_library(nomad-headers ${HPP_FILES} ${CPP_FILES})
