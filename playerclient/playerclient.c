#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <assert.h>
#include <ncurses.h>



#define C_REGISTER 0
#define C_START 1
#define C_MEETING 2
#define C_FINISHTASK 3

struct player_command {
  int type;
  char user[50];
  char data[100];
};

struct payload {
  int task_completion;

};

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

void send_all(int socket, char* buffer, size_t length) {
    char *ptr = (char*) buffer;
    while (length > 0)
    {
        int i = send(socket, ptr, length, 0);
        if (i < 1){
          perror("Could not send");
        }
        ptr += i;
        length -= i;
    }
}

int main(int argc, char* argv []) {
  int sid = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in srv;
  struct hostent *server = gethostbyname("localhost");

  srv.sin_family = AF_INET;
  srv.sin_port = htons(27182);
  memcpy(&srv.sin_addr.s_addr,server->h_addr,server->h_length);

  int status = connect(sid, (struct sockaddr*)&srv, sizeof(srv));

  struct player_command reg_comm;
  struct payload p;
  memset(&reg_comm, 0, sizeof(struct player_command));
  memset(&p, 0, sizeof(p));

  reg_comm.type = htonl(C_REGISTER);
  strcpy(reg_comm.user, argv[1]);
  strcpy(reg_comm.data, "");

  int ch;
  if(send(sid, &reg_comm, sizeof(reg_comm), 0) != -1) {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    char* role = "You are the Imposter\n";
    char* crewmate_comm = "[F1]-Quit [F2]-Call Meeting [F3]-Finish Task";
    char* taskbar_start = "Task Completion: |";
    int task_percentage = 0;

    attron(A_BOLD);
    mvprintw(0, (COLS/2)-(strlen(role)/2), role);
    attroff(A_BOLD);
    mvprintw(1, (COLS/2)-(strlen(crewmate_comm)/2), crewmate_comm);
    mvprintw(3, 0, taskbar_start);
    for(int i = strlen(taskbar_start); i < strlen(taskbar_start)+100; i++){
      mvaddch(3, i, '-');
    }
    mvprintw(3, strlen(taskbar_start)+100, "|");


    refresh();

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    attron(COLOR_PAIR(1));
    int i = 0;
    int given_perc = 50;
    int local_perc = 0;
    while(1) {

      fd_set readSet;
      FD_ZERO(&readSet);

      //Need to reead from both stdin and server fd
      FD_SET(sid, &readSet);
      FD_SET(0, &readSet);

      select(sid+1, &readSet, NULL, NULL, NULL);

      if(FD_ISSET(sid, &readSet)) {
         if(recv(sid, &p, sizeof(p), 0) != -1) {
           p.task_completion = ntohl(p.task_completion);
           start_color();
         	 init_pair(1, COLOR_RED, COLOR_WHITE);
           if(p.task_completion > task_percentage) {
             attron(COLOR_PAIR(1));
             for(int i = task_percentage; i < task_percentage+(p.task_completion-task_percentage);i++){
               mvprintw(3, strlen(taskbar_start)+i, " ");
             }
             attroff(COLOR_PAIR(1));
           } else {
             for(int i = task_percentage; i >= task_percentage - (task_percentage-p.task_completion);i--){
               mvprintw(3, strlen(taskbar_start)+i, "-");
             }
           }

           refresh();
           task_percentage = p.task_completion;
         }

       }

    }
    endwin();

  }



  return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx){
  WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);
	wrefresh(local_win);

	return local_win;
}

void destroy_win(WINDOW *local_win) {
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}
