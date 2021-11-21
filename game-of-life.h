#ifndef GAME_OF_LIFE
#define GAME_OF_LIFE

typedef struct gol gol;

struct gol
{
	char *output;
	char *buffer;

	int width;
	int height;
};

struct gol *gol_create	(int width, int height);
void gol_free				(struct gol *game);
void gol_update			(struct gol *game);
void gol_dump_map			(const struct gol *game);

char gol_getpos			(struct gol *game, int x, int y);
void gol_setpos			(struct gol *game, int x, int y, int state);

void gol_build_random	(struct gol *game, int seed);

#endif
