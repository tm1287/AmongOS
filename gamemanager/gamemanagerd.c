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

#define MAXLINE 100
#define OPTIONLEN 50
#define MAXCONFS 50

struct daemon_config {
  int port;
  char* options[MAXCONFS];
};

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

  //Brute force close open file descriptors
  int fd;
  for (fd = sysconf(_SC_OPEN_MAX); fd>=0; fd--)
  {
      close (fd);
  }

  //Start logging
  openlog ("gamemanagerdaemon", LOG_PID, LOG_DAEMON);
}

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

int main (int argc, char **argv) {
    struct daemon_config* config = (struct daemon_config*) malloc(sizeof(struct daemon_config));
    config->port = 0;

    char* init_options[MAXCONFS] = {"PORT"};
    memcpy(config->options, init_options, sizeof(config->options));

    read_conf(config, argv[1]);

    /*
    spawn_daemon();

    syslog (LOG_NOTICE, "Game Manager started");

    while(1){
      sleep(1);
    }

    syslog (LOG_NOTICE, "Game Manager stopped");
    closelog();
    */
    free(config);
    return 0;
}
