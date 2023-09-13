  /* Server code in C */
 
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <thread>
  #include <map>
  #include <vector>

  using namespace std;

  vector<int> client_sockets;

  void thread_read(int TSC){
    char message[256];
    while(true){
        bzero(message,256);
        int n = read(TSC,message,255);
        if (n < 0) perror("ERROR reading from socket");

        if(strcmp(message,"END")==0){
          printf("closed connection\n");
          close(TSC);
          for (vector<int>::iterator iter = client_sockets.begin(); iter != client_sockets.end(); ++iter){
            if(*iter == TSC){
              client_sockets.erase(iter);
              break;
            }
          }
        }
        int m;
        for (vector<int>::iterator it = client_sockets.begin(); it != client_sockets.end(); ++it) {
            m = write(*it,message,strlen(message));
            if (m < 0) perror("ERROR writing to socket");
        }
    }
  }
 

  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buffer[256];
    char buffer2[256];
    int n;
 
    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("error bind failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if(-1 == listen(SocketFD, 10))
    {
      perror("error listen failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    for(;;)
    {
      int  NSC = accept(SocketFD, NULL, NULL);
 
      if(0 > NSC)
      {
        perror("error accept failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
    client_sockets.push_back(NSC);

    thread(thread_read,NSC).detach();
      
    }
 
    close(SocketFD);
    return 0;
  }
