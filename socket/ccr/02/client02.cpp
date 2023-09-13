 /* Client code in C */
 
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <thread>
 
using namespace std;




 void client_read(int SFD)
{
    char buffer2[256];
    while(true){
    bzero(buffer2,256);
    int n = read(SFD,buffer2,256);
    if (n < 0) perror("ERROR reading from socket");

    printf("Message received: [%s]\n",buffer2);
    }
}

int main(int argc, char *argv[])
  {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
    char buffer[256];
 
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
 	
 	
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
 
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    thread(client_read,SocketFD).detach();

    char data[256];
     do{
      printf("Data to be send: ");
      fgets(data,256,stdin);
      if(strcmp(data, "END\n") == 0){
        write(SocketFD,"END",3);
        break;
      }
      write(SocketFD,data,strlen(data));
    }
    while(data != "END\n");
    
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);
    return 0;
  }
