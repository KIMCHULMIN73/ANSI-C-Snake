/******************************************
 **                                      **
 **          Snakes on NCURSES           **
 **                                      **
 ******************************************
 **             frontend.h               **
 ******************************************
 **          Julia Evans, 2013           **
 **     modified by kimchulmin, 2024     **
 ******************************************/

#include <ncurses.h>
#include <string.h>

/* prototype of functions related to displaying/drawing game data */
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
int new_game(WINDOW **gwd, Board **board, bool mode) ;
void draw_border(int ymax, int xmax);
void display_points(WINDOW **gwd, PointList* point, int x, int y, enum Flag erase);
void draw_info(int ymax, int xmax, int *score, int *snk_num, int *stage);
enum Ending outro(WINDOW **gwd, int ymax, int xmax);
