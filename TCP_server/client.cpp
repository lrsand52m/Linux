#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<string>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<strings.h>
#include<cstring>
#include<stdlib.h>
#include<pthread.h>
using namespace std;
void *Recv(void * arg)
{
	char buf[1024] = {0};
	int socket_fd = *(int*)arg;
	while(1)
	{
		ssize_t r_sz = 1;
      	  r_sz=read(socket_fd,buf,sizeof(buf)-1);
      	  if(r_sz>0)
        	{
      	      buf[r_sz]=0;
      	      cout<<buf;
        	}	
	}
}
int main(int argc,char* argv[])
{
    int socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd<0)
    {
        cerr<<"socket error!!!"<<endl;
        exit(1);
    }
    struct sockaddr_in peer;
    socklen_t len=sizeof(peer);
    bzero(&peer,len);
    peer.sin_family=AF_INET;
    peer.sin_port=htons(atoi(argv[2]));
    peer.sin_addr.s_addr=inet_addr(argv[1]);
    if(connect(socket_fd,(struct sockaddr*)&peer,len)<0)
    {
        cerr<<"connect error!!!"<<endl;
    }	
	pthread_t tid;
	pthread_create(&tid,0,Recv,(void*)&socket_fd);
    char buf[1024]={0};
    std::string message;
    while(1)
    {
		
	//cout<<"\nPlase Enter# ";
        cin>>message;
        write(socket_fd,message.c_str(),message.size());
    }
    close(socket_fd);
    return 0;
}
