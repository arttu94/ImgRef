#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "main.h"

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 200;

bool Init();

bool LoadMedia(const char* dir);

void Close();

SDL_Texture* LoadTexture(const char* dir);

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* texture = nullptr;

float WindowOpacity = 1.0f;

bool LockWindow = false;
bool TextureFillWindow = false;

bool Init()
{
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		window = SDL_CreateWindow("Reference Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP);
		if (window == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool LoadMedia(const char* dir)
{
	//Loading success flag
	bool success = true;

	//Load PNG texture
	texture = LoadTexture(dir);
	if (texture == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}

	return success;
}

void Close()
{
	//Free loaded image
	SDL_DestroyTexture(texture);
	texture = NULL;

	//Destroy window	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* LoadTexture(const char* dir)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(dir);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", dir, IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", dir, SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

int main(int argc, char* args[])
{
	SDL_bool done;
	SDL_Event event;

	char* dropped_filedir;

	Init();

	const char* tmp = "data/dropfileimage.png";
	LoadMedia(tmp);

	//SDL_SetWindowOpacity(window, 0.5f);

	WindowOpacity = 1.0f;

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	done = SDL_FALSE;

	while (!done)
	{
		while (!done && SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case (SDL_QUIT):
				done = SDL_TRUE;
				break;

			case (SDL_DROPFILE):
				dropped_filedir = event.drop.file;

				LoadMedia(dropped_filedir);

				SDL_SetWindowResizable(window, SDL_TRUE);

				//SDL_QueryTexture(texture, NULL, NULL, &w, &h);

				//SDL_SetWindowSize(window, w, h);

				SDL_free(dropped_filedir);
				break;

			case (SDL_MOUSEWHEEL):
				if (event.wheel.y > 0) // scroll up
				{
					if(WindowOpacity < 1.0f)
					WindowOpacity += 0.05f;
					SDL_SetWindowOpacity(window, WindowOpacity);
				}
				else if (event.wheel.y < 0) // scroll down
				{
					if(WindowOpacity > 0.1f)
						WindowOpacity -= 0.05f;
					SDL_SetWindowOpacity(window, WindowOpacity);
				}
				break;

			case (SDL_KEYDOWN):
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					done = SDL_TRUE;
				}
				else if (event.key.keysym.sym == SDLK_m)
				{
					SDL_MinimizeWindow(window);
				}
				else if (event.key.keysym.sym == SDLK_l)
				{
					LockWindow = !LockWindow;
					if (LockWindow)
					{
						SDL_SetWindowOpacity(window, WindowOpacity);
						SDL_SetWindowBordered(window, SDL_FALSE);
					}
					else
					{
						SDL_SetWindowOpacity(window, 1.0f);
						SDL_SetWindowBordered(window, SDL_TRUE);
					}
				}
				else if (event.key.keysym.sym == SDLK_f)
				{
					TextureFillWindow = !TextureFillWindow;
				}
				break;

			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_ENTER:
					break;
				case SDL_WINDOWEVENT_LEAVE:
					break;
				}
				break;


			default:
				break;
			}
		}

		//Clear screen
		SDL_RenderClear(renderer);

		int w, h;

		SDL_QueryTexture(texture, NULL, NULL, &w, &h);

		//Render texture to screen
		SDL_Rect dstrect = { 0, 0, w, h };
		SDL_RenderCopy(renderer, texture, NULL, TextureFillWindow ? &dstrect : NULL);

		//Update screen
		SDL_RenderPresent(renderer);
	}

	Close();

	return 0;
}