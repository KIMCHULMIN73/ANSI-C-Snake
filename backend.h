/******************************************
 **                                      **
 **          Snakes on NCURSES           **
 **                                      **
 ******************************************
 **              backend.h               **
 ******************************************
 **          kimchulmin, 2026.5          **
 ******************************************/

#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

/* MACRO for Snake Game */
#define MAX_STAGE      9
#define SNAKE_MAX      5
#define SNAKE_X        0
#define SNAKE_Y        0
#define WIDTH          COLS
#define HEIGHT         LINES
#define SCR_XMIN       20
#define SCR_YMIN       20
#define FRAME_RATE     60
#define FR_BASEMENT    80
#define SCALE          65536

/* enumeration */
enum Direction { UP, DOWN, LEFT, RIGHT, BACKWARD };    // to define direction of snake
enum Status { SUCCESS, FAILURE };                      // to define result(return) of functions
enum Ending { NEWGAME, CONTINUE, EXIT, NONE };         // to define game mode
enum Flag { ON, OFF };                                 // to define flag for any purpose

/* data structure to make linked-list for cell(Element of Snake, Enemy, Foods . . .) */
struct PointList                                       // structure to define linked-list cell (snake, food etc.)
{
  int x;                                               // x-positon of cell on game screen
  int y;                                               // y-positon of cell on game screen
  chtype symbol;                                       // shape of cell (chtype : 2-byte character to deal with NCURSES special characters)
  struct PointList* next;                              // point to next cell : linked-list
};

typedef struct PointList PointList;                    // redefine data type

/* data structure to save game informations */
typedef struct                                         // structure of Board to save real-time game status(data)
{
  PointList *snake;                                    // pointer to save head of snake
  PointList *foods;                                    // pointer to save first food
  PointList *enemy;                                    // pointer to save head of enemy
  int xmax;                                            // to save width of game screen
  int ymax;                                            // to save height of game screen
  int score;                                           // to save game score
  int snake_num;                                       // to save number of snakes remained
  int stage;                                           // to save current game stage
} Board;

/* prototype of functions related to game operations */
Board* create_board(PointList* snake, PointList* foods, PointList* enemy, int xmax, int ymax);
PointList* create_cell(int x, int y, chtype symbol);
PointList* create_snake(int x, int y);
PointList* create_enemy(int x, int y);
enum Status add_new_food(Board* board);
void delete_foods(Board* board);
void delete_board(Board* board);
int foods_counter(Board *board);
bool list_contains(PointList* cell, PointList* list);
bool is_same_place(PointList* cell1, PointList* cell2);
bool remove_from_list(PointList* snake_head, PointList** foods_list);
enum Direction get_key(enum Direction previous);
enum Status move_snake(Board* board, enum Direction dir, int *x, int *y);
PointList* next_move(Board* board, enum Direction dir);
enum Status move_enemy(Board* board, int *x, int *y);
PointList* next_move_enemy(Board* board);
void reloc_snake(PointList** snake, int x, int y, enum Direction *dir, int *snake_num);

/* prototype of auxiliary functions */
bool str2int(char string[], int *pint);
int int2str(int number, char* num_str);
int getstring(FILE *file, char *string);
int putstring(FILE *file, char *string);
