#include <math.h>
#include <SDL2/SDL.h>
#include "game-of-life.h"

#define SCREEN_WIDTH		700
#define SCREEN_HEIGHT	700

#define MAP_WIDTH			100
#define MAP_HEIGHT		100

typedef struct game_window game_window;

struct game_window
{
	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Event event;

	char running;
	char pause;
	char mouse_press;

	int delay;
	int mouse_pos_x;
	int mouse_pos_y;

	int width;
	int height;
};

int init_window(game_window *win, char *title, int width, int height);
void render(game_window *win, gol *game);

int init_window(game_window *win, char *title, int width, int height)
{
	int wflags = 0; // window flags
	int rflags = 0; // render flags

	win->width = width;
	win->height = height;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		return -1;

	win->window = SDL_CreateWindow(title,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height,
			wflags);

	if (!win->window)
	{
		win->running = 0;
		return -2;
	}

	win->renderer = SDL_CreateRenderer(win->window, -1, rflags);

	if (!win->renderer)
	{
		win->running = 0;
		return -3;
	}

	SDL_SetRenderDrawColor(win->renderer, 255, 255, 255, 255);
	SDL_RenderPresent(win->renderer);

	SDL_GetMouseState(&win->mouse_pos_x, &win->mouse_pos_y);
	win->mouse_press = 0;
	win->running = 1;
	win->pause = 0;
	win->delay = 100;

	return 0;
}

void destroy_window(game_window *win)
{
	// clean up
	SDL_DestroyWindow(win->window);
	SDL_DestroyRenderer(win->renderer);
	SDL_Quit();
}

void handle_events(game_window *win, gol *game)
{
	while (SDL_PollEvent(&(win->event)))
	{
		switch (win->event.type)
		{
			case SDL_QUIT:
				win->running = 0;
				break;

			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&win->mouse_pos_x, &win->mouse_pos_y);
				if (win->pause == 1 && win->mouse_press == 1 && win->event.button.button == SDL_BUTTON_LEFT)
				{
					int x = win->mouse_pos_x / ((float)win->width / (float)game->width);
					int y = win->mouse_pos_y / ((float)win->height / (float)game->height);
					gol_setpos(game, x, y, 1);
					render(win, game);
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (win->pause == 1 && win->mouse_press == 0 && win->event.button.button == SDL_BUTTON_LEFT)
				{
					win->mouse_press = 1;
					int x = win->mouse_pos_x / ((float)win->width / (float)game->width);
					int y = win->mouse_pos_y / ((float)win->height / (float)game->height);
					gol_setpos(game, x, y, 1);
					render(win, game);
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if (win->mouse_press == 1) win->mouse_press = 0;
				break;

			case SDL_KEYDOWN:
				switch (win->event.key.keysym.sym)
				{
					case SDLK_SPACE:
						win->pause = win->pause == 1 ? 0 : 1;
						break;
				};
				break;
		};
	}
}

void render(game_window *win, gol *game)
{
	SDL_RenderClear(win->renderer);

	// width and height of pixels
	int pixel_w = (win->width / game->width);
	int pixel_h = (win->height / game->height);

	for (int i = 0; i < game->width * game->height; i++)
	{
		// row and column
		int x = i % game->width;
		int y = (i - x) / game->width;
		if (gol_getpos(game, x, y) == 1)
		{
			SDL_Rect tile = { x * pixel_w, y * pixel_h, pixel_w, pixel_h };
			SDL_SetRenderDrawColor(win->renderer, 0, 0, 0, 255);
			SDL_RenderFillRect(win->renderer, &tile);
		}
	}

	SDL_SetRenderDrawColor(win->renderer, 255, 255, 255, 255);
	SDL_RenderPresent(win->renderer);
}

int main()
{
	game_window win;
	gol *game = gol_create(MAP_WIDTH, MAP_HEIGHT);
	init_window(&win, "game of life", SCREEN_WIDTH, SCREEN_HEIGHT);
	render(&win, game);

	while (win.running)
	{
		handle_events(&win, game);
		if (win.pause == 0)
		{
			gol_update(game);
			render(&win, game);
			SDL_Delay(win.delay);
		}
	}

	destroy_window(&win);
	gol_free(game);

	return 0;
}
