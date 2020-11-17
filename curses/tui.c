
#include <ncurses.h>
#include <string.h>

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

int main()
{
  int ch;

	initscr();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0);

  char* role = "You are the Imposter\n";
  char* crewmate_comm = "[F1]-Quit [F2]-Call Meeting [F3]-Finish Task\n";
  attron(A_BOLD);
  mvprintw(0, (COLS/2)-(strlen(role)/2), role);
  attroff(A_BOLD);
  mvprintw(1, (COLS/2)-(strlen(crewmate_comm)/2), crewmate_comm);
	refresh();

  while((ch = getch()) != KEY_F(1)) {

	}
	endwin();
	return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
  WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);
	wrefresh(local_win);

	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}
