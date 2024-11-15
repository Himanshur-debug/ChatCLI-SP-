#include<iostream>
#include<string>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<vector>

#define PORT 8080
#define BufferSize 1024

std::vector<int> ClientSockets;

void *HandleClient(void *arg){
 int ClientSocket = *(int*)arg;
 char Buffer[BufferSize];

 while(true){
  memset(Buffer, 0, BufferSize);
  int BytesReceived = recv(ClientSocket, Buffer, BufferSize, 0);
  if(BytesReceived<=0){
   std::cerr<<"Client Disconneted...."<<std::endl;
   close(ClientSocket);
   break;
  }

  std::cout<<"Client: "<<Buffer<<std::endl;

  for(int sock:ClientSockets){
   if(sock!=ClientSocket){
    send(sock,Buffer,strlen(Buffer),0);
   }
  }
 }
 return nullptr;
}

int main(){
 int ServerSocket, ClientSocket;
 struct sockaddr_in ServerAddr, ClientAddr;
 socklen_t AddrSize = sizeof(ClientAddr);

 ServerSocket = socket(AF_INET,SOCK_STREAM, 0);
 if(ServerSocket == -1){
  std::cerr<<"Failed to Open the socket"<<std::endl;
  return -1;
 }

 ServerAddr.sin_family = AF_INET;
 ServerAddr.sin_port = htons(PORT);
 ServerAddr.sin_addr.s_addr = INADDR_ANY;

 
 if(bind(ServerSocket,(struct sockaddr*)&ServerAddr, sizeof(ServerAddr))==-1){
  std::cerr<<"Binding Failed..."<<std::endl;
  return -1;
 }

 if(listen(ServerSocket, 5)==-1){
  std::cerr<<"Listening Failed..."<<std::endl;
  return -1;
 }

 std::cout<<"Server Listening on Port "<<PORT<<std::endl;

 while(true){
  ClientSocket = accept(ServerSocket,(struct sockaddr*)&ClientAddr, &AddrSize);
  if(ClientSocket==-1){
   std::cerr<<"Client Connection Failed..."<<std::endl;
   continue;
  }

  ClientSockets.push_back(ClientSocket);
  std::cout<<"Client Connected..."<<std::endl;

  pthread_t ClientThread;
  pthread_create(&ClientThread, nullptr, HandleClient, &ClientSocket);
 }

 close(ServerSocket);
 return 0;

}
