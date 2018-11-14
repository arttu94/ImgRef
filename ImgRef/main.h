#pragma once

bool Init(SDL_Window * &window, SDL_Surface * &screenSurface);

SDL_Surface* LoadSurface(char* &dir);

void Close();
