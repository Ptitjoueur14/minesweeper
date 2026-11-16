#define _POSIX_C_SOURCE 200809L

#include "../include/options.hpp"
#include "../include/events.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *trim(char *line)
{
    if (!line)
    {
        return NULL;
    }

    while (*line != '\0' && isspace(*line))
    {
        line++;
    }

    size_t i = strlen(line) - 1;
    while (line[i] != '\0' && isspace(line[i]))
    {
        line[i] = '\0';
        i++;
    }

    return line;
}

char *parse_option(FILE *file)
{
    char *buffer;
    if (getline(&buffer, nullptr, file) == -1)
    {
        fclose(file);
        return nullptr;
    }

    char *option;
    char *line = strtok_r(buffer, "=", &option);
    if (!line)
    {
        return NULL;
    }

    return trim(line);
}

struct options *parse_options(const char *filename)
{
    struct options *options = malloc(sizeof(struct options));
    if (!options)
    {
        return nullptr;
    }

    // Set keys by default
    options->click_key = SDLK_a;
    options->flag_key = SDLK_q;
    
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        return options;
    }

    // If config file is valid, parse keys
    char *click_line = parse_option(file);
    if (click_line != NULL)
    {
        options->click_key = getSDLKeyCode(click_line[0]);
    }
    char *flag_line = parse_option(file);
    if (flag_line != NULL)
    {
        options->flag_key = getSDLKeyCode(flag_line[0]);
    }

    return options;
}
