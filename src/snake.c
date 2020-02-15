
#include <gb/gb.h>
#include <rand.h>

#include "snake-map.h"
#include "snake-tiles.h"

#define NUM_SPRITES 13
#define SPRITE_SIZE 8
#define MAX_SNAKE_SIZE 10

#define LEFT_WALL_TILE 3
#define RIGHT_WALL_TILE 18
#define TOP_WALL_TILE 5
#define BOTTOM_WALL_TILE 17

// Sprite ID for the snake components start at 0 (for the head) and increase
// with each segment created until MAX_SNAKE_SIZE.
#define SNAKE_HEAD_SPRITE_ID 0
#define SNAKE_TILE 1

#define FRUIT_SPRITE_ID 31
#define FRUIT_TILE 2

// Helper macro to snap pixel positions to a 8x8 tile.
#define TILE_COORD(t) t* SPRITE_SIZE

// Helper macros to generate a random x,y co-ordinate within the walls.
#define RANDOM_X (rand() & 14) + LEFT_WALL_TILE
#define RANDOM_Y (rand() & 11) + TOP_WALL_TILE

// Cardinality of the snake.
typedef enum Direction { UP, DOWN, LEFT, RIGHT };

struct Fruit {
  UINT8 x, y;
};

struct SnakeSegment {
  UINT8 x, y;
};

struct Snake {
  UINT8 x, y, size;
  enum Direction direction;
  struct SnakeSegment segment[MAX_SNAKE_SIZE];
};

// Initial x,y coordinates will be set during init()
struct Snake snake = {0, 0, 1, LEFT};

// Initial x,y coordinates will be set during init() and randomize every time
// eaten.
struct Fruit fruit = {0, 0};

UINT8 isGameOver() {
  UINT8 i;

  // If hit the left or right wall.
  if ((snake.x < LEFT_WALL_TILE) || (snake.x > RIGHT_WALL_TILE)) {
    return 1;
  }

  // If hit the top or bottom wall.
  if ((snake.y < TOP_WALL_TILE) || (snake.y > BOTTOM_WALL_TILE)) {
    return 1;
  }

  // If have eaten self.
  for (i = 0; i < snake.size - 1; i++) {
    if ((snake.x == snake.segment[i].x) && (snake.y == snake.segment[i].y)) {
      return 1;
    }
  }
  return 0;
}

void drawSnake() {
  UINT8 i;

  move_sprite(SNAKE_HEAD_SPRITE_ID, TILE_COORD(snake.x), TILE_COORD(snake.y));

  for (i = snake.size - 1; i > 0; i--) {
    move_sprite(i, TILE_COORD(snake.segment[i].x),
                TILE_COORD(snake.segment[i].y));
  }
}

void moveSnake() {
  UINT8 i;

  // First segment moves to the position where the snake head was.
  snake.segment[0].x = snake.x;
  snake.segment[0].y = snake.y;

  // Shift the rest of the segments up one to the x,y position of the previous.
  for (i = snake.size - 1; i > 0; --i) {
    snake.segment[i].x = snake.segment[i - 1].x;
    snake.segment[i].y = snake.segment[i - 1].y;
  }

  // Move snake head in the correct direction.
  switch (snake.direction) {
    case UP:
      snake.y--;
      break;
    case RIGHT:
      snake.x++;
      break;
    case LEFT:
      snake.x--;
      break;
    case DOWN:
      snake.y++;
      break;
  }
}

void growSnake() { set_sprite_tile(snake.size++, SNAKE_TILE); }

void moveFruit() {
  // Generate a new x,y pair for the Fruit within the borders.
  // TODO: make sure this doesn't spawn where a snake segment exists.
  fruit.x = RANDOM_X;
  fruit.y = RANDOM_Y;
  move_sprite(FRUIT_SPRITE_ID, TILE_COORD(fruit.x), TILE_COORD(fruit.y));
}

UINT8 hasEatenFruit() {
  return ((snake.x == fruit.x) && snake.y == fruit.y) ? 1 : 0;
}

void init() {
  SPRITES_8x8;

  set_bkg_data(0, NUM_SPRITES, tiles);
  set_sprite_data(0, NUM_SPRITES, tiles);
  set_bkg_tiles(0, 0, mapWidth, mapHeight, map);

  set_sprite_tile(FRUIT_SPRITE_ID, FRUIT_TILE);
  set_sprite_tile(SNAKE_HEAD_SPRITE_ID, SNAKE_TILE);

  initarand(DIV_REG);

  // Set initial placement of fruit.
  moveFruit();

  // Set initial placement of Snake.
  snake.x = RANDOM_X;
  snake.y = RANDOM_Y;

  SHOW_BKG;
  SHOW_SPRITES;
}

void main() {
  UINT8 frame = 0;
  UINT8 level = 1;
  UINT8 score = 1;

  init();

  while (1) {
    wait_vbl_done();
    frame++;

    // Pause the game when gameover.
    // TODO: create game over screen and allow restart of game.
    if (isGameOver() == 1) {
      return;
    }

    if (hasEatenFruit() == 1) {
      score++;

      // Every 10th score increase the level.
      if (score % 10 == 0) {
        level++;
      }

      if (snake.size < MAX_SNAKE_SIZE) {
        growSnake();
      }

      moveFruit();
    }

    // Prevent moving in the opposite direction.
    if (joypad() & J_RIGHT && snake.direction != LEFT) {
      snake.direction = RIGHT;
    }
    if (joypad() & J_LEFT && snake.direction != RIGHT) {
      snake.direction = LEFT;
    }
    if (joypad() & J_UP && snake.direction != DOWN) {
      snake.direction = UP;
    }
    if (joypad() & J_DOWN && snake.direction != UP) {
      snake.direction = DOWN;
    }

    drawSnake();

    // Move faster every time the level increases gradually towards moving every
    // tick.
    if (frame % (10 - level) == 0) {
      moveSnake();
    }
  }
}
