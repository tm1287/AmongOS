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

int main(int argc, char* argv []) {
  int sid = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in srv;
  struct hostent *server = gethostbyname("localhost");

  srv.sin_family = AF_INET;
  srv.sin_port = htons(27182);
  memcpy(&srv.sin_addr.s_addr,server->h_addr,server->h_length);

  int status = connect(sid, (struct sockaddr*)&srv, sizeof(srv));
  printf("%d\n", status);

  while(1) {
    sleep(2);
  }
  
  return 0;
}
