
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
int main(){
  char buf[256] = {0};
  char command[] = "uptime | sed 's/.*up  \\([^,]*\\).*/\\1/'";
  char greeting_text[128];
  printf("What is your name?\n");
  scanf("%s\n",buf);
  strcpy(greeting_text, "Hello, dear ");
  strcat(greeting_text, buf);
  printf("%s\n", greeting_text);
  printf("This computer has been running for ");
  printf("\n%s\n", command);
  printf("\n%s\n", buf);
  // system(command);
  return 0;
}