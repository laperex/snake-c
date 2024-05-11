#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_video.h"
#include <SDL_ttf.h>
#include <time.h>

#define WIDTH (1920)
#define HEIGHT (1080)
#define TITLE "snake"
#define FULLSCREEN false


#define SNAKE_BUFFER_LIMIT 100
#define BOX_WIDTH 40


static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Event event;


static SDL_Point snake[SNAKE_BUFFER_LIMIT];
static SDL_Point apple;
static SDL_Point direction, snake_dir;
static int snake_len = 0;

static int head;

void render(int box_width, int xoff, int yoff, int width, int height) {
	SDL_RenderClear(renderer);

	{
		static SDL_Rect box = {};
		
		box.w = box_width;
		box.h = box_width;

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		
		box.x = apple.x * BOX_WIDTH + xoff;
		box.y = apple.y * BOX_WIDTH + yoff;
		
		SDL_RenderFillRect(renderer, &box);
		

		for (int i = 0; i < snake_len; i++) {
			int v = i + head;

			v -= (v >= SNAKE_BUFFER_LIMIT) ? SNAKE_BUFFER_LIMIT: 0;

			if (v == head) {
				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			} else {
				SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			}

			box.x = snake[v].x * BOX_WIDTH + xoff;
			box.y = snake[v].y * BOX_WIDTH + yoff;

			SDL_RenderFillRect(renderer, &box);
		}
		
		SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
		for (int x = 0; x < WIDTH; x += BOX_WIDTH) {
			for (int i = 0; i < 7; i++) {
				SDL_RenderDrawLine(renderer, x + i + xoff, 0, x + i + xoff, height);
			}
		}

		for (int y = 0; y < HEIGHT; y += BOX_WIDTH) {
			for (int i = 0; i < 7; i++) {
				SDL_RenderDrawLine(renderer, 0, y + i + yoff, width, y + i + yoff);
			}
		}

		// Don't forget to free your surface and texture
	}

	SDL_RenderPresent(renderer);

	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
}


int events() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			return 1;
		}

		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.scancode == SDL_SCANCODE_W) {
				direction.y = -1;
				direction.x = 0;
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_A) {
				direction.x = -1;
				direction.y = 0;
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_S) {
				direction.y = 1;
				direction.x = 0;
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_D) {
				direction.x = 1;
				direction.y = 0;
			}
		}
	}
	
	return 0;
}


void apple_regen() {
	while (1) {
		apple.x = (rand() % (WIDTH / BOX_WIDTH));
		apple.y = (rand() % (HEIGHT / BOX_WIDTH));
		
		for (int i = 0; i < snake_len; i++) {
			if (apple.x == snake[i].x || apple.y == snake[i].y) {
				continue;
			}
		}

		return;
	}
}


int update() {
	if (snake_len == 0) {
		snake_len = 5;

		for (int i = 0; i < 5; i++) {
			snake[5 - i - 1].x = i + 1;
			snake[5 - i - 1].y = 2;
		}

		apple_regen();

		head = 0;

		direction.x = 1;
		direction.y = 0;
	} else {
		static struct SDL_Point old_dir;
		struct SDL_Point new_head;
		
		for (int i = 0; i < 2; i++) {
			new_head.x = snake[head].x + direction.x;
			new_head.y = snake[head].y + direction.y;


			if (new_head.x > WIDTH / BOX_WIDTH - 1) {
				new_head.x = 0;
			}

			if (new_head.y > HEIGHT / BOX_WIDTH - 1) {
				new_head.y = 0;
			}


			if (new_head.x < 0) {
				new_head.x = WIDTH / BOX_WIDTH - 1;
			}

			if (new_head.y < 0) {
				new_head.y = HEIGHT / BOX_WIDTH - 1;
			}

			if (new_head.x != snake[head + 1].x || new_head.y != snake[head + 1].y) {
				head = head > 0 ? head - 1: SNAKE_BUFFER_LIMIT - 1;
				
				snake[head].x = new_head.x;
				snake[head].y = new_head.y;
				
				old_dir.x = direction.x;
				old_dir.y = direction.y;
			
				break;
			}

			direction.x = old_dir.x;
			direction.y = old_dir.y;
		}
		
		for (int i = 1; i < snake_len; i++) {
			int v = i + head;

			v -= (v >= SNAKE_BUFFER_LIMIT) ? SNAKE_BUFFER_LIMIT: 0;
			
			if (snake[v].x == snake[head].x && snake[v].y == snake[head].y) {
				printf("game over!\n");
				
				snake_len = 0;
				
				break;
			}
		}


		if (apple.x == snake[head].x && apple.y == snake[head].y) {
			snake_len += 1;
			
			apple_regen();
			
			printf("points: %d\n", snake_len - 5);
		}
	}

	return 0;
}


int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	while (1) {
		if (events()) {
			break;
		}

		update();

		render(BOX_WIDTH, 0, 0, WIDTH, HEIGHT);
		
		SDL_Delay(60.0);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}