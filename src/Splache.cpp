/**
 *  Splache.cpp
 *  
 *
 *  Created by Derek Parker and Kyle Nusbaum.
 */

#include "Splache.h"

/*TEMPORARY*/
//#include <sys/socket.h>
//#include <netinet/in.h>
//#define MAXTHREADS 10


static void daemonize();
void *spawn_thread(void*);

int main(int argc, char* argv[]) 
{  
  // PATH_TO_LOGFILE will be replaced with a configparser class member <map>
  char* PATH_TO_TRAFFICLOG = (char*)"../logs/splache.traffic";
  char* PATH_TO_LOGFILE = (char*)"../logs/splache.log";

  pthread_mutex_t errorLoglock;
  pthread_mutex_t trafficLoglock;
  if(pthread_mutex_init(&errorLoglock,NULL) != 0)
    printf("Error creating mutex.");
  if(pthread_mutex_init(&trafficLoglock,NULL) != 0)
    printf("Error creating mutex.");
  
  Log errorLogger(PATH_TO_LOGFILE, &errorLoglock);
  Log trafficLogger(PATH_TO_TRAFFICLOG, &trafficLoglock);

  //daemonize after logs start to give them a chance to report errors.
  daemonize();

  try 
    {
      ServerSocket server(80);     
 
      while (true) 
        {
	  ServerSocket *sock = new ServerSocket();
	  server.accept(sock);
	  Session *s = new Session(sock, &trafficLogger, &errorLogger);
	  pthread_t newThread;
	  int error = pthread_create(&newThread,NULL,spawn_thread,(void*)s);
	  if(error != 0)
	    printf("Thread Error");
	  //spawn_thread(s);

        }
    }
  
  catch (SocketException *e) 
    {
      e->logExceptionToFile(errorLogger);
      delete(e);
    }
  
  return 0;
}

static void daemonize()
{
  pid_t pid;
  pid = fork();
  if(pid < 0)
    exit(-1);
  
  //If original process, exit.
  if(pid > 0)
    exit(0);

  //If we are executing this, we are the child process.
  //Change the file usermask.
  umask(0);
  
  //Create new group and make child the leader
  if(setsid() < 0)
    exit(1);
  
  //Change working directory. (Eventually pull this from config?)
  if(chdir("/") < 0)
    exit(1);

  //Redirect standard IO streams to null.
  freopen("/dev/null/", "r", stdin);
  freopen("/dev/null/", "w", stdout);
  freopen("/dev/null/", "w", stderr);
  
}

void *spawn_thread(void *arg)
{
  printf("Spawning Thread\n");
  Session *session = (Session*)arg;
  session->run();
  delete(session);
  printf("Thread dying.\n");
}
/*
static pthread_t getThread()
{
  if(threadQueue.empty)
    
}
static p
*/
