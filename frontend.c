/******************************************
 **                                      **
 **          Snakes on NCURSES           **
 **                                      **
 ******************************************
 **             frontend.c               **
 ******************************************
 **          Julia Evans, 2013           **
 **     modified by kimchulmin, 2024     **
 ******************************************/

#include <ncurses.h>
#include "backend.h"
#include "frontend.h"

/* to create NCURSES window on which game could be played */    
WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);

    wrefresh(local_win);

    return local_win;
}

/* to destroy NCURSES window */
void destroy_win(WINDOW *local_win)
{
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    wrefresh(local_win);

    delwin(local_win);
}

/* to initialize information to start totally new game or just clear stage */    
int new_game(WINDOW **gwd, Board **board, bool mode)
{
    int xmax, ymax;
    int stage, index;
    char temp_ch[2], *msg;

    getmaxyx(*gwd, ymax, xmax);        // get size of game window
    wclear(*gwd);                      // clear game window

    if( mode == true )                 // init totally new game 
    {
        // create new board & new snake, new enemy for new game
        *board = create_board(create_snake(SNAKE_X, SNAKE_Y), NULL, create_enemy(xmax-1, ymax-1), xmax, ymax);

        msg = "Select STAGE you want (1~9)";
        mvwprintw(*gwd,ymax/2, (xmax-strlen(msg))/2, msg);       // print guidence message
        wrefresh(*gwd);

        temp_ch[0] = getch();                                    // to get '1 ~ 9' number as user input
        temp_ch[1] = '\0';
        if (!str2int(temp_ch, &stage)) stage = MAX_STAGE / 2;    // input isn't a number between 1 and 9, stage would be fixed to the middle one.

        (*board)->stage = stage;
    }
    else
    {
        msg = "STAGE CLEAR . . . Push any button to continue";
        mvwprintw(*gwd, ymax/2, (xmax-strlen(msg))/2, msg);
        wrefresh(*gwd);
        stage = (*board)->stage++;
        timeout(-1);                                                              // set timer-out to wait user-input until user pushes any key...
        getch();
    }

    wclear(*gwd);

    for (index = 0; index < (xmax/SCR_XMIN)*(ymax/SCR_YMIN)*stage ; index++)    // register and draw new foods according to stage(game level)
        add_new_food(*board);

    timeout(FRAME_RATE*(MAX_STAGE-stage)+FR_BASEMENT);                          // set game-speed ; timer which waits user-input for certain period of time according to the stage(game level) 

    return SUCCESS;
}

/* to draw border of game screen */    
void draw_border(int ymax, int xmax)
{
    int index_x, index_y;
    int max_x, max_y;

    max_x = xmax-1;
    max_y = ymax-2;

    mvaddch(0, 0, ACS_ULCORNER);
    mvaddch(0, max_x, ACS_URCORNER);
    mvaddch(max_y, 0, ACS_LLCORNER);
    mvaddch(max_y, max_x, ACS_LRCORNER);

    for( index_x = 1 ; index_x < max_x ; index_x++ )
    {
        mvaddch(0, index_x, ACS_HLINE);
        mvaddch(max_y, index_x, ACS_HLINE);
    }

    for( index_y = 1 ; index_y < max_y ; index_y++ )
    {
        mvaddch(index_y, 0, ACS_VLINE);
        mvaddch(index_y, max_x, ACS_VLINE);
    }
}

/* to display cell(basic element, which can be part of snake, enemy, food . . .) on game screen */    
void display_points(WINDOW **gwd, PointList* point, int x, int y, enum Flag erase)
{
    chtype symbol;

    while(point)
    {
        if (erase == ON) symbol = ' ';
        else symbol = point->symbol;

        mvwaddch(*gwd, point->y, point->x, symbol);
        point = point->next;
    }

    mvwaddch(*gwd, y, x, ' '); 
}

/* to display current game info. */    
void draw_info(int ymax, int xmax, int *score, int *snk_num, int *stage)
{
    int index;
    char scr[10], snk[10], stg[10];

    int2str(*score, scr);
    int2str(*snk_num, snk);
    int2str(*stage, stg);    

    mvaddstr(ymax-1, 3, "SCORE");
    mvaddstr(ymax-1, 8, scr);

    mvaddstr(ymax-1, 14, "SNAKE");
    mvaddstr(ymax-1, 20, snk);

    mvaddstr(ymax-1, 24, "STAGE");
    mvaddstr(ymax-1, 30, stg);
}

/* to dispay outro and get keypad input when game is over */      
enum Ending outro(WINDOW **gwd, int ymax, int xmax)
{
    char *msg, ch;

    wclear(*gwd);
    msg = "Game Over";
    mvwprintw(*gwd, ymax/2, (xmax-strlen(msg))/2, msg);
    msg = "Push N(New Game) or C(Continue) or E(EXIT)";
    mvwprintw(*gwd, ymax/2+2, (xmax-strlen(msg))/2, msg);
    wrefresh(*gwd);

    timeout(-1);    // set timer-out to wait user-input until user pushes any key...
    ch = getch();
    switch(ch)
    {
        case 'n' :
        case 'N' : return NEWGAME;
        case 'c' :
        case 'C' : return CONTINUE;
        case 'e' :
        case 'E' : return EXIT;
        default  : return NONE;
    }
}
