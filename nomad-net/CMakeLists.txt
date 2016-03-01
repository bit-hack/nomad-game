file(GLOB HPP_FILES source/*.h)
file(GLOB CPP_FILES source/*.cpp)

add_library(nomad-net ${HPP_FILES} ${CPP_FILES})

if(WIN32)
  target_link_libraries(nomad-net wsock32 ws2_32)
endif()
