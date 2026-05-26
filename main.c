/******************************************
 **                                      **
 **          Snakes on NCURSES           **
 **                                      **
 ******************************************
 **                main.c                **
 ******************************************
 **          kimchulmin, 2026.5          **
 ******************************************/

#include <ncurses.h>
#include <string.h>
#include "backend.h"
#include "frontend.h"

enum Status main(void)
{
  WINDOW *game_scr;                           // structure pointer to handle game screen(window)
  Board *board;                               // structure pointer to save game data
  int win_x, win_y, win_width, win_height;    // location & size of game window
  bool on_game, on_board;                     // flag of game status
  int foods_num;                              // total number of foods on each stage
  enum Direction dir;                         // direction of the snake
  int snake_tail_x, snake_tail_y;             // previous location of tail of snake
  int enemy_tail_x, enemy_tail_y;             // previous location of tail of snake
  
  // initialize NCURSES to use : to understand these codes, find out KLDP how-to pages
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_WHITE, COLOR_BLACK);  
  refresh();
  
  win_y = 1, win_x = 1, win_height = HEIGHT-3, win_width = WIDTH-3;
  game_scr = create_newwin(win_height, win_width, win_y, win_x);                  // create window(game screen)
  
  on_board = true;                                                                // flag of totally new game(board)
  
  while(on_board)                                                                 // main loop of totally new game                                           
  {
    new_game(&game_scr, &board, true);                                            // totally init game information to start new game
    dir = RIGHT;                                                                  // initial direction of snake
    
    draw_border(HEIGHT, WIDTH);

    on_game = true;                                                               // flag of a game

    while(on_game)                                                                // main loop of a game  
    {
      foods_num = foods_counter(board);                                           // count all foods on game screen
      
      if(foods_num == 0 && board->stage < MAX_STAGE)                              // condition of stage clear : snake has eaten all foods on the game screen
      {
        new_game(&game_scr, &board, false);                                       // init game to clear stage
        display_points(&game_scr, board->snake, snake_tail_x, snake_tail_y, ON);
        reloc_snake(&board->snake, SNAKE_X, SNAKE_Y, &dir, NULL);                 // relocate snake to clear stage
        display_points(&game_scr, board->enemy, enemy_tail_x, enemy_tail_y, ON);
        reloc_snake(&board->enemy, win_width-1, win_height-1, NULL, NULL);        // relocate enemy to clear stage                
      }
      
      dir = get_key(dir);                                                         // get user(keypad) input by timeout-time period that depends on each game stage(level)

      // move snake & enemy and check ending condition
      if(move_snake(board, dir, &snake_tail_x, &snake_tail_y) == FAILURE || move_enemy(board, &enemy_tail_x, &enemy_tail_y) == FAILURE)    
      {
        if(board->snake_num > 0)
        {
          display_points(&game_scr, board->snake, snake_tail_x, snake_tail_y, ON);
          reloc_snake(&board->snake, SNAKE_X, SNAKE_Y, &dir, &board->snake_num);                     // if snakes are remained, continue game
        }
        else
          switch (outro(&game_scr, board->ymax, board->xmax))                                        // display outro image & get command to do post-process after 'game over'
          {
            case NEWGAME  : delete_board(board);                                                     // delete all context of current game
                            on_game = false;                                                         // quit curent game cotext
                            break;
                            
            case CONTINUE : display_points(&game_scr, board->snake, snake_tail_x, snake_tail_y, ON);
                            reloc_snake(&board->snake, SNAKE_X, SNAKE_Y, &dir, NULL);                // relocate snake to continue game
                            display_points(&game_scr, board->enemy, enemy_tail_x, enemy_tail_y, ON);
                            reloc_snake(&board->enemy, win_width-1, win_height-1, NULL, NULL);       // relocate enemy to clear stage 
                            board->snake_num = SNAKE_MAX;                                            // init number of snakes to continue game
                            timeout( FRAME_RATE * (MAX_STAGE - board->stage) + FR_BASEMENT );        // configue timer to set a new shoter timeout period for next stage
                            break;
            case EXIT     :
            case NONE     :
            default       : on_game = false;                                                         // quit curent game cotext
                            on_board = false;                                                        // quit total game to end this program
                            break;
          }
      }

      wattron(game_scr, COLOR_PAIR(1)|A_BOLD);
      display_points(&game_scr, board->snake, snake_tail_x, snake_tail_y, OFF);                      // display snake
      
      wattron(game_scr, COLOR_PAIR(2)|A_BOLD);
      display_points(&game_scr, board->enemy, enemy_tail_x, enemy_tail_y, OFF);                      // display enemy
            
      wattron(game_scr, COLOR_PAIR(3)|A_BOLD);        
      display_points(&game_scr, board->foods, snake_tail_x, snake_tail_y, OFF);                      // display foods 

      wrefresh(game_scr);

      draw_info(HEIGHT, WIDTH, &board->score, &board->snake_num , &board->stage);                    // display game information

      refresh();
    }
  }

  // deinitialize NCURSES to go back to linux shell
  endwin();

  return SUCCESS;
}
