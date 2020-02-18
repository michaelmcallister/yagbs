
#include <gb/gb.h>
#include <rand.h>

#include "gameover.h"
#include "snake-map.h"
#include "snake-tiles.h"

// Game mechanics.
#define SPRITE_SIZE 8
#define MAX_LEVEL 10

// Tile X,Y coords of where the walls are for collision detection.
#define LEFT_WALL_TILE 2
#define RIGHT_WALL_TILE 17
#define TOP_WALL_TILE 3
#define BOTTOM_WALL_TILE 15

// Tile IDs used throughout game.
#define SNAKE_TILE 1
#define FRUIT_TILE 2
#define CLEAR_TILE 0x7F
// Starting tile for numbers. Tile #3 = '0', Tile #4 = '1' and so on.
#define NUMBER_TILE 3

// Fruit and score are the only sprites. Score sprites are between 12-15.
// Snake segments are drawn as backgrounds.
#define FRUIT_SPRITE_ID 11
#define SCORE_SPRITE_ID 12

// Helper macro to snap pixel positions to a 8x8 tile.
#define X_COORD(x) (x + 1) * SPRITE_SIZE
#define Y_COORD(y) (y + 2) * SPRITE_SIZE

// Helper macros to generate a random x,y co-ordinate within the walls.
#define RANDOM_X (rand() & 14) + LEFT_WALL_TILE
#define RANDOM_Y (rand() & 11) + TOP_WALL_TILE

// Cardinality of the snake.
typedef enum Direction { UNDEFINED, UP, DOWN, LEFT, RIGHT };

UINT8 frame, level, score;

struct Fruit {
  UINT8 x, y;
};

struct SnakeSegment {
  UINT8 x, y;
};

struct Snake {
  UINT8 x, y, size;
  enum Direction direction;
  struct SnakeSegment segment[100];
};

// Initial x,y coordinates will be set during init()
struct Snake snake = {0, 0, 0, UNDEFINED};

// Initial x,y coordinates will be set during init() and randomize every time
// eaten.
struct Fruit fruit = {0, 0};

void drawScore() {
  UINT8 sprite_id = SCORE_SPRITE_ID;
  UINT8 x = 19;
  UINT8 n = score;
  UINT8 digit, i;

  // set score tiles to 0000.
  for (i = 0; i < 4; i++) {
    set_sprite_tile(sprite_id, NUMBER_TILE);
    move_sprite(sprite_id, X_COORD(x), Y_COORD(0));
    x--;
    sprite_id++;
  }

  // Override the tiles starting from the right, and moving to the left
  // for the score, starting at the least significant bit.
  sprite_id = SCORE_SPRITE_ID;
  while (n) {
    digit = n % 10;
    n /= 10;

    set_sprite_tile(sprite_id, digit + NUMBER_TILE);
    sprite_id++;
  }
}

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
  UINT8 i = snake.size - 1;
  UINT8 tile = CLEAR_TILE;

  // Clear the tail.
  set_bkg_tiles(snake.segment[i].x, snake.segment[i].y, 1, 1, &tile);

  tile = SNAKE_TILE;
  set_bkg_tiles(snake.x, snake.y, 1, 1, &tile);
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

void moveFruit() {
  // Generate a new x,y pair for the Fruit within the borders.
  UINT8 done = 0;
  UINT8 i;
  while (!done) {
    fruit.x = RANDOM_X;
    fruit.y = RANDOM_Y;
    done = 1;
    // If the new fruit position is where the snake head is...
    if (snake.x == fruit.x && snake.y == fruit.y) {
      done = 0;
      break;
    }
    // ...or where one of the snake segments are, then regenerate the x,y pair
    // and try again.
    for (i = 0; i < snake.size - 1; i++) {
      if (fruit.x == snake.segment[i].x && fruit.y == snake.segment[i].y) {
        done = 0;
        break;
      }
    }
  }

  move_sprite(FRUIT_SPRITE_ID, X_COORD(fruit.x), Y_COORD(fruit.y));
}

UINT8 hasEatenFruit() {
  return (snake.x == fruit.x && snake.y == fruit.y) ? 1 : 0;
}

void changeDirection() {
  UINT8 j = joypad();
  // Prevent moving in the opposite direction.
  if (j & J_RIGHT && snake.direction != LEFT) {
    snake.direction = RIGHT;
  }
  if (j & J_LEFT && snake.direction != RIGHT) {
    snake.direction = LEFT;
  }
  if (j & J_UP && snake.direction != DOWN) {
    snake.direction = UP;
  }
  if (j & J_DOWN && snake.direction != UP) {
    snake.direction = DOWN;
  }
}

void init() {
  frame = 0;
  level = 1;
  score = 0;

  // Set initial placement of Snake.
  snake.x = RANDOM_X;
  snake.y = RANDOM_Y;
  snake.size = 1;
  snake.direction = UNDEFINED;

  DISPLAY_OFF;
  SPRITES_8x8;

  set_bkg_data(0, 0, tiles);
  set_sprite_data(0, 0, tiles);
  set_bkg_tiles(0, 0, mapWidth, mapHeight, map);

  set_sprite_tile(FRUIT_SPRITE_ID, FRUIT_TILE);

  initarand(DIV_REG);

  moveFruit();
  drawSnake();
  drawScore();

  SHOW_BKG;
  SHOW_SPRITES;
  DISPLAY_ON;
  // Only start moving once a directional button has been pressed.
  waitpad(J_RIGHT | J_LEFT | J_UP | J_DOWN);
}

void main() {
  init();
  while (1) {
    wait_vbl_done();
    drawSnake();
    frame++;

    // Pause the game when gameover. Pressing any non-directional button will
    // reset.
    if (isGameOver()) {
      set_sprite_tile(FRUIT_SPRITE_ID, CLEAR_TILE);
      set_bkg_tiles(0, 0, gameoverWidth, gameoverHeight, gameover);
      waitpad(J_A | J_B | J_START | J_SELECT);
      init();
    }

    if (hasEatenFruit()) {
      score++;
      snake.size++;

      // Every 10th score increase the level.
      if (score % 10 == 0 && level < MAX_LEVEL) {
        level++;
      }

      moveFruit();
      drawScore();
    }

    changeDirection();

    // Move faster every time the level increases gradually towards moving every
    // tick.
    if (frame % (10 - level) == 0) {
      moveSnake();
    }
  }
}
