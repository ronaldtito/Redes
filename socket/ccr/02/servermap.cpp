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

  map<string,int> client_sockets;

  void thread_read(int TSC,string nickname){
    char buffer[256];
    while(true){
        bzero(buffer,256);
        int n = read(TSC,buffer,255);
        if (n < 0) perror("ERROR reading from socket");

        if(strcmp(buffer,"END")==0){
          printf("closed connection\n");
          close(TSC);
          client_sockets.erase(nickname);
        }
      buffer[n]= '\0';
      string message(buffer);
      size_t comaPos = message.find(':');
      if(comaPos != string::npos){
        string destino = message.substr(0,comaPos);
        string mensaje = nickname+": "+message.substr(comaPos +1);

        /*for(map<string,int>::iterator iter = client_sockets.begin(); iter != client_sockets.end();++iter){
          if(iter->first == destino ){
            int m = write(iter->second,mensaje.c_str(),mensaje.size());
            break;
          }
        }*/
        int m = write(client_sockets[destino],mensaje.c_str(),mensaje.size());
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
      buffer[n-1]= '\0';
      string clientNickname(buffer);
      
      if(clientNickname.empty()){
      	perror("no nickname");}
      
      cout<< buffer <<" se conecto "<<endl;
      client_sockets[buffer]=NSC;
      	
      thread(thread_read,NSC,buffer).detach();
      
    }
 
    close(SocketFD);
    return 0;
  }