#include<iostream>
#include<string>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include<pthread.h>

#define PORT 8080
#define BufferSize 1024

void* receive_messages(void* arg){
 int ClientSocket = *(int*)arg;
 char Buffer[BufferSize];

 while(true){
  memset(Buffer, 0, BufferSize);
  int BytesReceived = recv(ClientSocket, Buffer, BufferSize, 0);
  if(BytesReceived<=0){
   std::cerr<<"Disconnected from Server..."<<std::endl;
   close(ClientSocket);
   exit(1);
  }
  std::cout<<"Server: "<<Buffer<<std::endl;
 }
 return nullptr;
}

int main(){
 int ClientSocket;
 struct sockaddr_in ServerAddr;
 char Buffer[BufferSize];

 ClientSocket = socket(AF_INET,SOCK_STREAM,0);
 if(ClientSocket==-1){
  std::cerr<<"Failed to create Socket..."<<std::endl;
  return -1;
 }
 
 ServerAddr.sin_family = AF_INET;
 ServerAddr.sin_port = htons(PORT);
 inet_pton(AF_INET, "127.0.0.1",&ServerAddr.sin_addr);

 if(connect(ClientSocket,(struct sockaddr*)&ServerAddr, sizeof(ServerAddr))==-1){
  std::cerr<<"Connection Failed..."<<std::endl;
  return -1;
 }
 std::cout<<"Connection Established..."<<std::endl;


 pthread_t ReceivedThread;
 pthread_create(&ReceivedThread, nullptr, receive_messages, &ClientSocket);

 while(true){
  std::string Message;
  std::getline(std::cin,Message);
  send(ClientSocket, Message.c_str(), Message.size(), 0);
 }
 close(ClientSocket);
 return 0;

}