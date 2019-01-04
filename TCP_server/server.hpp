#pragma once
#include<vector>
#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<strings.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<string>
#include<cstring>
#include<sys/wait.h>
#include<pthread.h>
#include<stdio.h>
using namespace std;
typedef struct sockaddr_in sockaddr_in;
class Server;
struct Pthread_data
{
	int id;
    Server* pServer;
    int accept_fd;
};

class Sock
{
public:
    Sock(const int &port)
        :sock_fd(-1),_port(port)
    {
        ;
    }
    void Socket()
    {
        sock_fd=socket(AF_INET,SOCK_STREAM,0);
        if(sock_fd<0)
        {
            cerr<<"socket error!!!"<<endl;
            exit(1);
        }
    }
    void Bind()
    {
        struct sockaddr_in local;
        bzero(&local,sizeof(local));
        local.sin_family=AF_INET;
        local.sin_port=htons(_port);
        local.sin_addr.s_addr=htonl(INADDR_ANY);
        if(bind(sock_fd,(struct sockaddr*)&local,sizeof(local))<0)
        {
            cerr<<"bind error!!!"<<endl;
            exit(2);
        }
    }
    void Listen()
    {
        if(listen(sock_fd,5)<0)
        {
            cerr<<"listen error!!!"<<endl;
            exit(3);
        }
    }
    int Accept()
    {
        struct sockaddr_in peer;
        socklen_t len=sizeof(peer);
        int accept_fd=accept(sock_fd,(struct sockaddr*)&peer,&len);
        if(accept_fd<0)
        {
            cerr<<"accept error!!!"<<endl;
            return -1;
        }
        return accept_fd;
    }
private:
    int sock_fd;
    int _port;
};

class Server
{
 public:
     Server(const int &port)
         :sock(port){}
		 
     void InitServer()
     {
         sock.Socket();
         sock.Bind();
         sock.Listen();
     }
	 
	 void Chatwith(int id,int sendfd,int recvfd)
	 {
		 string msg("the user ");
		 msg+=(char)(id+'0');
		 msg+=" is chat with you!\n";
		 write(recvfd,msg.c_str(),msg.size());
		 msg = "";
		 msg+="user ";
		 msg+=(char)(id+'0');
		msg+=": ";
		 char buf[1024]={0};
		 while(1)
         {
             ssize_t r_sz=read(sendfd,buf,sizeof(buf)-1);
             if(r_sz==0)
             {
                 cout<<"客户端退出！！！"<<endl;
                 break;
             }
             else if(r_sz>0)
             {
                 buf[r_sz]='\n';
		     buf[r_sz+1] = 0;
			msg+=buf;
		    write(recvfd,msg.c_str(),msg.size());
		 	msg = "";
			 msg+="user ";
		 	msg+=(char)(id+'0');
			msg+=": ";
             }
             else
             {
                 cerr<<"read error!!!"<<endl;
                 break;
             }
         }
	 }
     void Service(int fd,int id)
     {
         	char buf[1024] = {0};
		 char p[64] = {0};
		 sprintf(p,"server: your user id is: %d\n",id);
		 write(fd,p,strlen(p));
		 while(1)
		 {
			write(fd,"server: please choose user who you want to chat\n",48);
			ssize_t r_sz=read(fd,buf,sizeof(buf)-1);
			if(r_sz==0)
			{
				cout<<"客户端退出！！！"<<endl;
				break;
			}
			else if(r_sz>0)
			{
				buf[r_sz]=0;
				int i = atoi(buf);
				
				if(i>=clients.size()||i<0)
				{
					write(fd,"server: the user does not exist!\n",34);
					continue;
				}
				else 
				{
					Chatwith(id,fd,clients[i]);
					continue;
				}
			}
		 }
         close(fd);
     }
     static void* ThreadRun(void* arg)
     {
         pthread_detach(pthread_self());
         Pthread_data* p=(Pthread_data*)arg;
         Server* pServer=p->pServer;
         int accept_fd=p->accept_fd;
		 int id = p->id;
         delete p;
         pServer->Service(accept_fd, id);
     }
     void Run()
     {
         while(1)
         {
             int accept_fd = sock.Accept();
             if(accept_fd<0)
             {
                 continue;
             }
             cout<<"连接到一个新客户端！！"<<endl;
			 clients.push_back(accept_fd);
             Pthread_data* p=new Pthread_data;
			 p->id = clients.size()-1;
             p->pServer=this;
             p->accept_fd = accept_fd;
             pthread_t tid;
             pthread_create(&tid,NULL,ThreadRun,p);
         }
     }
private:
     Sock sock;
	 vector<int> clients;
};
