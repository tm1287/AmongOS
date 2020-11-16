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

int main (int argc, char **argv) {
    spawn_daemon();


    while(1){
      syslog (LOG_NOTICE, "Game Manager started");
      sleep(20);
      break;
    }

    syslog (LOG_NOTICE, "Game Manager stopped");
    closelog();

    return 0;
}
