#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <stdio.h>

struct options
{
  SDL_Keycode click_key;
  SDL_KeyCode flag_key;
};

char *trim(char *line);

char *parse_option(FILE *file);

struct options *parse_options(const char *filename);

#endif /* ! OPTIONS_H */
