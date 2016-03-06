file(GLOB HPP_FILES source/*.h)
file(GLOB CPP_FILES source/*.cpp)

add_executable(nomad ${HPP_FILES} ${CPP_FILES})
target_link_libraries(nomad PUBLIC
    nomad-headers
    nomad-game
    nomad-human
    nomad-ai
    nomad-net
    nomad-objects
    nomad-util
    nomad-window
    nomad-server
    ${SDL_LIBRARY})
