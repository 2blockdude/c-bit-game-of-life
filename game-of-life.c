#include <stdio.h>
#include <stdlib.h>

#include "game-of-life.h"

struct gol *gol_create(int width, int height)
{
	struct gol *game = malloc(sizeof(struct gol));
	game->output = calloc(width * height / (sizeof(char) * 8) + 1, sizeof(char));
	game->buffer = calloc(width * height / (sizeof(char) * 8) + 1, sizeof(char));

	game->width = width;
	game->height = height;

	return game;
}

void gol_free(struct gol *game)
{
	free(game->output);
	free(game->buffer);
	free(game);
}

void gol_update(struct gol *game)
{
	int byte_size = sizeof(char) * 8;

	// update alive state
	for (int i = 0; i < game->height * game->width; i++)
	{
		// foundation for bit minipulation
		int addr = i / byte_size;
		int shift = (byte_size - 1) - (i % byte_size);

		// set buffer bit
		game->buffer[addr] &= ~(1 << shift);
		game->buffer[addr] += (game->output[addr] & (1 << shift));

		// gets columns and rows
		int x = i % game->width;			// column
		int y = (i - x) / game->width;	// row

		// wrap around left / right
		int left = (x - 1) < 0 ? game->width - 1 : x - 1;
		int right = (x + 1) % game->width;

		// wrap around above / below
		int above = (y - 1) < 0 ? game->height - 1 : y - 1;
		int below = (y + 1) % game->height;

		int neighbors =
			// get above
			gol_getpos(game, left, above) +
			gol_getpos(game, x, above) +
			gol_getpos(game, right, above) +
			// get middle
			gol_getpos(game, left, y) +
			gol_getpos(game, right, y) +
			// get below
			gol_getpos(game, left, below) +
			gol_getpos(game, x, below) +
			gol_getpos(game, right, below);

		// cell rules
		if (neighbors >= 4 || neighbors <= 1)
			game->buffer[addr] &= ~(1 << shift);	// set to zero
		else if (neighbors == 3)
			game->buffer[addr] |= 1 << shift;		// set to one

	}

	// swap output and buffer
	char *temp = game->output;
	game->output = game->buffer;
	game->buffer = temp;
}

void gol_dump_map(const struct gol *game)
{
	for (int i = 0; i < game->height * game->width; i++)
	{
		switch (game->output[i])
		{
			case 0:
				printf(" ");
				break;

			case 1:
				printf("#");
				break;
		};

		if ((i % game->width) == (game->width - 1))
			printf("\n");
	}
}

char gol_getpos(struct gol *game, int x, int y)
{
	int byte_size = sizeof(char) * 8;
	int linear_pos = y * game->width + x;
	int shift_amount = (byte_size - 1) - (linear_pos % byte_size);
	int addr = linear_pos / byte_size;
	return (game->output[addr] >> shift_amount) & 1;
}

void gol_setpos(struct gol *game, int x, int y, int state)
{
	int byte_size = sizeof(char) * 8;
	int linear_pos = y * game->width + x;
	int shift_amount = (byte_size - 1) - (linear_pos % byte_size);
	int addr = linear_pos / byte_size;
	game->output[addr] &= ~(1 << shift_amount);
	game->output[addr] += state << shift_amount;
}

void gol_build_random(struct gol *game, int seed)
{
	srand(seed);
	for (int i = 0; i < game->height * game->width / 8; i++)
		game->output[i] = game->buffer[i] = rand() % 256;
}
