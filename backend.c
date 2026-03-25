/******************************************
 **                                      **
 **          Snakes on NCURSES           **
 **                                      **
 ******************************************
 **              backend.c               **
 ******************************************/
 
#include "backend.h"
#include "frontend.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

int snake_ate_food = 0;

/* to mount new board(data structure for game information) onto real memory  */
Board* create_board(PointList* snake, PointList* foods, PointList* enemy, int xmax, int ymax)
{
  Board* board = malloc(sizeof(*board));    // allocating board type memory in HEAP to save game information

  // initializing data in board structure
  board->foods = foods;
  board->snake = snake;
  board->enemy = enemy;
  board->xmax = xmax;
  board->ymax = ymax;
  board->snake_num = SNAKE_MAX;
  board->score = 0;
  board->stage = 1;

  return board;
}

/* to mount one cell(PointList type linked-list data structure) onto real memory */
PointList* create_cell(int x, int y, chtype symbol)
{
  PointList* cell = malloc(sizeof(*cell));    // allocating PointList type memory in HEAP to save cell information.

  cell->x = x;
  cell->y = y;
  cell->symbol = symbol;
  cell->next = NULL;
  
  return cell;
}

/* to create a snake conisted of 2 cell */
PointList* create_snake(int x, int y)
{
  PointList* head = create_cell(x, y, ACS_RARROW);    // basic snake consists of head cell & tail cell only
  PointList* tail = create_cell(x, y, ACS_RARROW);
  head->next = tail;                                  // making linked list between head & tail

  return head;
}

/* to create a enemy-snake conisted of 2 cell */
PointList* create_enemy(int x, int y)
{
  PointList* head = create_cell(x, y, ACS_CKBOARD);    // basic snake consists of head cell & tail cell only
  PointList* tail = create_cell(x, y, ACS_CKBOARD);
  head->next = tail;                                   // making linked list between head & tail
  return head;
}

/* to create a food made of cell to make linked-list of foods */
enum Status add_new_food(Board* board)
{
  PointList* new_food;
  
  // creating a new food on random position
  new_food = create_cell(rand()%(board->xmax-4)+2, rand()%(board->ymax-4)+2, ACS_DIAMOND);
  
  // checking a new food whther conflict to other cells(food, snake) or not
  if (list_contains(new_food, board->foods) || list_contains(new_food, board->snake))
  {
    free(new_food);
    return FAILURE;
  }
  else 
  {
    new_food->next = board->foods;    // making linked list among foods
    board->foods = new_food;
    return SUCCESS;
  }
}

/* to delete all foods on linked-list */
void delete_foods(Board* board)
{
  PointList *CurP, *NexP;

  // deleting all cells of foods linked list
  if(board->foods != NULL) 
  {
    CurP = board->foods;
    while(CurP!=NULL)
    {
      CurP->x = 0;
      CurP->y = 0;
      NexP = CurP->next;
      free(CurP);
      CurP = NexP;
    }
  }
}

/* to delete a board from real memory */
void delete_board(Board* board)
{
  PointList *CurP, *NexP;

  // to de-initialize data in board data structure
  board->xmax = 0;
  board->ymax = 0;
  board->snake_num = 0;
  board->score = 0;
  board->stage = 0;

  delete_foods(board);       // deleting all foods on linked-list


  // deleting all cells of snakes on linked list
  if(board->snake != NULL)
  {
   CurP = board->snake;
   while(CurP!=NULL)
   {
      CurP->x = 0;
      CurP->y = 0;
      NexP = CurP->next;
      free(CurP);
      CurP = NexP;
   }
  }

  // deleting all cells of enemies on linked list
  if(board->enemy != NULL)
  {
    CurP = board->enemy;
    while(CurP!=NULL)
    {
      CurP->x = 0;
      CurP->y = 0;
      NexP = CurP->next;
      free(CurP);
      CurP = NexP;
    }
  }

  free(board);
  board = NULL;
}

/* to count number of foods remains */
int foods_counter(Board *board)
{
  int foods_count = 0;
  PointList *CurP, *NexP;
        
  CurP = board->foods;
  while(CurP!=NULL)
  {
    NexP = CurP->next;
    CurP = NexP;
    foods_count++;
  }

  return foods_count;
}

/* to check a list contains a cell or not through comparing the cells to all cells in the linked list */
bool list_contains(PointList* cell, PointList* list)
{
  PointList* s = list;
  while (s)
  {
    if (is_same_place(s, cell)) return true;
    s = s->next;
  }
  
  return false;
}

/* to compare to each other between positions of 2 cells */
bool is_same_place(PointList* cell1, PointList* cell2)
{
  return (cell1->x == cell2->x && cell1->y == cell2->y);
}

/* to remove a food(cell) from foods linked list */
bool remove_from_list(PointList* snake_head, PointList** foods_list)
{
  PointList *currP, *prevP;
  prevP = NULL;

  for (currP = *foods_list ; currP != NULL ; prevP = currP, currP = currP->next)
  {
    if (is_same_place(snake_head, currP))
    {
      if (prevP == NULL) *foods_list = currP->next;
      else prevP->next = currP->next;
      free(currP);
      return true;
    }
  }
  
  return false;
}

/* to get keypad input and decide direction of head of snake */      
enum Direction get_key(enum Direction previous)
{
  int ch = getch();
  switch (ch) {
    case KEY_LEFT   : if (previous != RIGHT) return LEFT;
                      else return BACKWARD;
                      
    case KEY_RIGHT  : if (previous != LEFT) return RIGHT;
                      else return BACKWARD;
                      
    case KEY_DOWN   : if (previous != UP) return DOWN;
                      else return BACKWARD;
                      
    case KEY_UP     : if (previous != DOWN) return UP;
                      else return BACKWARD;
                      
    default         : return previous;
  }
}

/* to move a snake on the game window */        
enum Status move_snake(Board* board, enum Direction dir, int *x, int *y)
{
  PointList *head, *tail;

  // Create a new head  
  head = next_move(board, dir);
  if (head == NULL)
    return FAILURE;

  // Check for collision with snake itself
  if (list_contains(head, board->snake))
    return FAILURE;

  // Check for collision with enemy
  if (list_contains(head, board->enemy))
    return FAILURE;

  // Check for collision with foods
  if (list_contains(head, board->foods))
  {
    // Remove the food collides with snake head from foods linked list
    remove_from_list(head, &(board->foods));
    snake_ate_food = 1;
    board->score+=10;
  }
  else 
  {
    // Cut off the tail from snake linked list
    tail = board->snake;
    while(tail->next->next)
      tail = tail->next;
      
    *x = (tail->next)->x;
    *y = (tail->next)->y;
      
    free(tail->next);
    tail->next = NULL;
  }

  // Attach the new head to previous snake
  head->next = board->snake;
  board->snake = head;
  
  return SUCCESS;
}

/* to create a new head(point) of snake by direction that player inputs */
PointList* next_move(Board* board, enum Direction dir)
{
  PointList* snake = board->snake;
  int next_x = snake->x;
  int next_y = snake->y;
  chtype next_symbol = snake->symbol;
  
  switch(dir)
  {
    case UP     : next_y--;
                  next_symbol = ACS_UARROW;
                  break;
    case DOWN   : next_y++;
                  next_symbol = ACS_DARROW;
                  break;
    case LEFT   : next_x--;
                  next_symbol = ACS_LARROW;
                  break;
    case RIGHT  : next_x++;
                  next_symbol = ACS_RARROW;
                  break;
    case BACKWARD : ;
  }
  
  if ( next_x < SNAKE_X || next_y < SNAKE_Y || next_x >= board->xmax || next_y >= board->ymax )
    return NULL;
  else
    return create_cell(next_x, next_y, next_symbol);
}

/* to move a enemy on the game window */        
enum Status move_enemy(Board* board, int *x, int *y)
{
  PointList *head, *tail;

  // Create a new head   
  head = next_move_enemy(board);
  if (head == NULL)
    return FAILURE;
  
  // Check for collision with snake
  if (list_contains(head, board->snake))
    return FAILURE;
  
  // Check for collision between snake and foods. If snake ate a food, lengths of both snake & enemy will be increased.
  if(snake_ate_food == 1) snake_ate_food = 0;
  else
  {
    // Cut off the tail from enemy linked list
    tail = board->enemy;
    while(tail->next->next)
      tail = tail->next;

    *x = (tail->next)->x;
    *y = (tail->next)->y;
            
    free(tail->next);
    tail->next = NULL;
  }  

  // Attach the new head to previous enemy
  head->next = board->enemy;
  board->enemy = head;
  
  return SUCCESS;
}

/* to create a new head(point) of enemy by random direction */
PointList* next_move_enemy(Board* board)
{
  PointList* enemy = board->enemy;
  int next_x = enemy->x;
  int next_y = enemy->y;
  static enum Direction dir = LEFT;
	time_t the_time;
  static int randx = 0, step = 0, dir_random = 2, max_step = 5;

  if (step++ > max_step)
  {
    step = 0;
	  time(&the_time);
	  randx = the_time % SCALE;
	  randx = (randx * 25173 + 13849) % 65536;		
	  dir_random = randx % 4;
    max_step = randx % 30;

    switch (dir_random)
    {
      case 0 : dir = UP; break;
      case 1 : dir = DOWN; break;
      case 2 : dir = LEFT; break;
      case 3 : dir = RIGHT; break;
    }
  }

  if (next_x < 1) dir = RIGHT;
  if (next_x > board->xmax - 1) dir = LEFT;
  if (next_y < 1) dir = DOWN;
  if (next_y > board->ymax - 1) dir = UP;
  
  switch(dir) 
  {
    case UP : next_y--; break;
    case DOWN : next_y++; break;
    case LEFT : next_x--; break;
    case RIGHT : next_x++; break;
  }
  
  return create_cell(next_x, next_y, ACS_CKBOARD);
}

/* to initialize start location of a snake */
void reloc_snake(PointList** snake, int x, int y, enum Direction *dir, int *snake_num)
{
  PointList *index;
    
  for( index = *snake ; index->next != NULL; index = index->next )
  {
    index->x = x;
    index->y = y;
  }
  
  if(dir != NULL) *dir = RIGHT;
  
  if( snake_num != NULL ) *snake_num = *snake_num - 1;
}

/* 4 auxiliary functions : string-to-integer, integer-to-string, get-string, put-string */

bool str2int(char string[], int *pint)
{
	int index = 0, sign = 1;
	*pint = 0;
	
	if( string[index] == '+' || string[index] == '-')
		sign = (string[index++] == '+') ? 1 : -1;
	
	while( '0' <= string[index] && string[index] <= '9' )
	    *pint = 10 * (*pint) + (string[index++] - '0');
	
	if( index != 0 && string[index] == '\0' )
	{
		*pint = sign * (*pint);
		return true;
	}
	else return false;
}

int int2str(int number, char* num_str)
{
	float num = (float)number;
	int i = 0, pwr = 1;
	char temp_ch[256];

    while( (int)(num/pwr) > (float)((1/3)*3) ) {
        temp_ch[i++] = '0' + (int)num % (pwr*10) / pwr;
		pwr *= 10;
	}
  
  if(number == 0) temp_ch[i++] = '0';
	
	while( i > 0 )
	    *(num_str++) = temp_ch[--i];
        
	*num_str = '\0';

    return SUCCESS;
}

int getstring(FILE *file, char *string)
{	
	int cond = true;

	while (cond) {
	*(string) = getc(file);
    if ( *(string++) == '\0' ) cond = false;
    else cond = true;
    }
    	
	return SUCCESS;
}

int putstring(FILE *file, char *string)
{
	while( *(string) != '\0' ) {
		putc(*string++, file);
	}
	putc('\0', file);
	
	return SUCCESS;
}



