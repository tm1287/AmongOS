/*
* gamemanagerd.c
* Daemon skeleton learned from https://nullraum.net/how-to-create-a-daemon-in-c/
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <dirent.h>

#define MAXLINE 100
#define OPTIONLEN 50
#define MAXCONFS 50

struct daemon_config {
  int port;
  char* options[MAXCONFS];
};

static void read_conf(struct daemon_config* config, char* conf_file) {
  //printf("%s\n", conf_file);
  FILE *fp;
  char buf[MAXLINE];
  char option[OPTIONLEN];
  int val = 0;

  if ((fp=fopen(conf_file, "r")) == NULL) {
    printf("Failed to read conf, daemon not starting...\n");
    exit(1);
  }


  ssize_t read;
  char* line = NULL;
  size_t len = 0;
  int option_index = 0;
  while((read = getline(&line, &len, fp)) != -1){
    strcpy(buf, line);

    sscanf(buf, "%s %d", option, &val);

    while(config->options[option_index]!=NULL) {
      if(strcmp(config->options[option_index], option) == 0){
        if(strcmp(option, "PORT") == 0){
          config->port = val;
        }
        break;
      }
      option_index++;
    }

    memset(buf, 0, MAXLINE);
    memset(option, 0, OPTIONLEN);
    val = 0;
  }
  fclose(fp);
  free(line);

}

static int create_socket_and_listen(struct daemon_config* config) {
  int sfd = socket(PF_INET,SOCK_STREAM,0);

  int flags = fcntl(sfd, F_GETFL, 0);
  flags = flags | O_NONBLOCK;
  fcntl(sfd, F_SETFL, flags);

  int enable = 1;
  setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR, &enable, sizeof(int));

  struct sockaddr_in gm_addr;
  gm_addr.sin_family = AF_INET;
  gm_addr.sin_port   = htons(config->port);
  gm_addr.sin_addr.s_addr = INADDR_ANY;

  bind(sfd,(struct sockaddr*)&gm_addr,sizeof(gm_addr));
  listen(sfd,10);
  syslog(LOG_NOTICE, "Listening on socketID %d", sfd);

  return sfd;
}

static void spawn_daemon() {
  pid_t pid = fork();

  if (pid < 0){
      //Failed, exit
      exit(1);
  } else if (pid > 0) {
      //In the parent, terminate the parent
      exit(0);
  }

  //Create new session
  if (setsid() < 0) {
    //Failed to set SessID
    exit(1);
  }

  //Catch but ignore signals
  signal(SIGCHLD, SIG_IGN);
  signal(SIGHUP, SIG_IGN);

  pid = fork();

  //Fork again to get rid of session leading process
  if (pid < 0) {
    //Failed to fork
    exit(1);
  }

  if (pid > 0){
    //Kill the parent again
    exit(0);
  }

  //Set filemode mask to 0666
  umask(0);

  //Situate the daemon in root dir
  chdir("/");

  //Close stdin/stdout,stderr
  int devnull = open("/dev/null", O_RDWR);
  dup2(devnull, STDIN_FILENO);
  dup2(devnull, STDOUT_FILENO);
  dup2(devnull, STDERR_FILENO);

  //Start logging
  openlog ("gamemanagerdaemon", LOG_PID, LOG_DAEMON);
}

int main (int argc, char **argv) {

    fd_set master;
    fd_set read_fds;
    int fdmax;

    int newfd;
    struct sockaddr_storage clientaddr;
    socklen_t clientaddrlen;

    char playercommandbuf[256];
    int numbytes;

    struct daemon_config* config = (struct daemon_config*) malloc(sizeof(struct daemon_config));
    config->port = 0;

    char* init_options[MAXCONFS] = {"PORT"};
    memcpy(config->options, init_options, sizeof(config->options));

    read_conf(config, argv[1]);

    spawn_daemon();
    syslog (LOG_NOTICE, "Game Manager started");

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    int sid = create_socket_and_listen(config);

    FD_SET(sid, &master);
    fdmax = sid;

    while(1){
      read_fds = master;

      select(fdmax+1, &read_fds, NULL, NULL, NULL);

      for(int rfd = 0; rfd <= fdmax; rfd++){
        if(FD_ISSET(rfd, &read_fds)) {
          if(rfd == sid) {
            clientaddrlen = sizeof clientaddr;
            newfd = accept(sid, (struct sockaddr*)&clientaddr, &clientaddrlen);

            FD_SET(newfd, &master);
            if(newfd > fdmax){
              fdmax = newfd;
            }
            syslog(LOG_NOTICE, "Got a new client");

          } else {
            memset(&playercommandbuf, 0, 100);
            if ((numbytes = recv(rfd, playercommandbuf, sizeof playercommandbuf, 0)) <= 0){
              if (numbytes == 0){
                syslog(LOG_NOTICE, "Client hung up");
              } else {
                syslog(LOG_ERR, "Error receiving data");
              }
              close(rfd);
              FD_CLR(rfd, &master);
            } else {
              //Got data from client
              syslog(LOG_NOTICE, playercommandbuf);
            }
          }
        }
      }

    }

    syslog (LOG_NOTICE, "Game Manager stopped");
    closelog();

    free(config);
    return 0;
}
