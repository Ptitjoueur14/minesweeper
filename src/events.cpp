#include "../include/events.hpp"

SDL_KeyCode getSDLKeyCode(const char c)
{
  if (c == 'a')
  {
    return SDLK_a;
  }
  if (c == 'q')
  {
    return SDLK_q;
  }
  return SDLK_UNKNOWN;
}
