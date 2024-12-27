#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#define DEBUG
#ifdef DEBUG
#define DEBUG_OUTPUT printf
#else
#define DEBUG_OUTPUT(...) do{} while(0)
#endif
int parse(int sockfd)
{
  char greeting_text[128];
  char buf[256] = {0};
  // Redirect stdout and stdin to the socket
  if (dup2(sockfd, STDOUT_FILENO) < 0) {
    perror("dup2");
  }
  if (dup2(sockfd, STDIN_FILENO) < 0) {
    perror("dup2");
  }
  DEBUG_OUTPUT("%p %p\n", buf, greeting_text);
  printf("What is your name?\n");
  fflush(stdout);
  fgets(buf, sizeof(buf), stdin);
  strcpy(greeting_text, "Hello, dear ");
  strcat(greeting_text, buf);
  printf("%s\n", greeting_text);
  return 0;
}
int main(int argc, char** argv)
{
  int connfd=0;
      parse(connfd);
	return 0;

}
