#pragma once


#include <SDL_rwops.h>
#include <SDL_surface.h>

SDL_Surface *IMG_Load_XCF_Layer(SDL_RWops *src, int which_layer);
