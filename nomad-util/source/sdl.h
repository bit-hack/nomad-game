#pragma once

#define _SDL_main_h
#if defined(__linux__)
  #include <SDL/SDL.h>
#elif defined(WIN32)
  #include <SDL.h>
#endif
#undef _SDL_main_h
