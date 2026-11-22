#define _POSIX_C_SOURCE 200809L

#include <fstream>
#include "../include/options.hpp"
#include "../include/events.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

std::string trim(std::string &line)
{
    size_t i = 0;
    while (line[i] != '\0' && isspace(line[i]))
    {
        i++;
    }

    size_t j = line.length() - 1;
    while (line[j] != '\0' && isspace(line[j]))
    {
        j--;
    }

    return line.substr(i, j);
}

char parse_option(std::ifstream &file)
{
    std::string line;
    std::getline(file, line);

    if (line.empty())
    {
        return 0;
    }

    return line.back();
}

Options::Options(const char *filename)
{
    // Set keys by default
    click_key = SDLK_a;
    flag_key = SDLK_q;

    if (!filename)
    {
        return;
    }
    
    std::ifstream file;
    file.open(filename);

    // If config file is valid, parse keys
    char click_char = parse_option(file);
    char flag_char = parse_option(file);

    click_key = getSDLKeyCode(click_char);
    flag_key = getSDLKeyCode(flag_char);

    file.close();
}
