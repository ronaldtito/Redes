  /* Server code in C */
  #include <iostream>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <cstdlib>
  #include <cstring>
  #include <unistd.h>
  #include <thread>
  #include <map>

  using namespace std;

  map<int,string> client_sockets;

  void thread_read(int TSC){
    char buffer[256];
    while(true){
        bzero(buffer,256);
        int n = read(TSC,buffer,255);
        if (n < 0) perror("ERROR reading from socket");

        if(strcmp(buffer,"END")==0){
          printf("closed connection\n");
          close(TSC);
          client_sockets.erase(TSC);
        }
      buffer[n]= '\0';
      string message(buffer);
      size_t comaPos = message.find(',');
      if(comaPos != string::npos){
        string destino = message.substr(0,comaPos);
        string mensaje = message.substr(comaPos +1);

        for(map<int,string>::iterator iter = client_sockets.begin(); iter != client_sockets.end();++iter){
          if(iter->second == destino ){
            int m = write(iter->first,mensaje.c_str(),mensaje.size());
            break;
          }
        }
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
      bzero(buffer,256);
      n = read(NSC,buffer,255);
      string clientInfo(buffer);
      cout<<buffer<<endl;
      size_t comaPos = clientInfo.find(',');
      if(comaPos != string::npos){
        string clientNick = clientInfo.substr(0,comaPos);
        client_sockets[NSC]=clientNick;
        
        cout<<clientNick<<" se conecto "<<endl;

        thread(thread_read,NSC).detach();
      }
      
    }
 
    close(SocketFD);
    return 0;
  }
