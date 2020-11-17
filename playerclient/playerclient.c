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

#define C_START 0;
#define C_MEETING 1;
#define C_FINISHTASK 2;

struct player_command {
  int type;
  char user[50];
  char data[100];
};

int main(int argc, char* argv []) {
  int sid = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in srv;
  struct hostent *server = gethostbyname("localhost");
  char serverdata[1000];


  srv.sin_family = AF_INET;
  srv.sin_port = htons(27182);
  memcpy(&srv.sin_addr.s_addr,server->h_addr,server->h_length);

  int status = connect(sid, (struct sockaddr*)&srv, sizeof(srv));

  while(1) {

  }

  return 0;
}
