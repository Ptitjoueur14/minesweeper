#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <stdio.h>

class Options
{
  public:
    SDL_Keycode click_key;
    SDL_KeyCode flag_key;

    Options(const char *filename);
};

char *trim(char *line);

char *parse_option(FILE *file);

Options parse_options(const char *filename);

#endif /* ! OPTIONS_H */
