#pragma once 
#include"threadpool.hpp"
#include"upload.hpp"
#include<sys/socket.h>
#include<unistd.h>
#include<iostream>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/stat.h>
#define MAX_LISTEN 5
#define MAX_THREAD 4
class HttpServer
{
    //建立服务端，接收新连接
    //为新连接创建线程池任务，添加到线程池
private:
    int _serv_sock;
    ThreadPool* _tp;


    static bool (HttpHandler)(int sock)//http任务的处理函数
    {
        RequestInfo info;
        HttpRequest req(sock);
        HttpResponse rsp(sock);
        if(!req.RecvHttpHeader(info))
        {
            goto out;
        }
        if(!req.ParseHttpHeader(info))
        {
            goto out;
        }
        if(info.RequestIsCGI())
        {
            rsp.CGIHandler(info);
        }
        else 
        {
            rsp.FileHandler(info);
        }
        close(sock);
        return true;
out:
        rsp.ErrHandler(info);
        close(sock);
        return false;
    }
public:
    HttpServer()
        :_serv_sock(-1),_tp(NULL){}
    bool Init(std::string &ip,int port)//完成服务端的建立，线程池的初始化
    {
        _serv_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(_serv_sock<0)
        {
            LOG("sock error\n");
            return false;
        }
        sockaddr_in laddr;
        laddr.sin_family = AF_INET;
        laddr.sin_port = htons(port);//不能使用htonl
        laddr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(sockaddr_in);
        if(bind(_serv_sock,(sockaddr*)&laddr,len)<0)
        {
            LOG("bind error\n");
            return false;
        }
        if(listen(_serv_sock,MAX_LISTEN)<0)
        {
            LOG("listen error\n");
            return false;
        }
        _tp = new ThreadPool(MAX_THREAD);
        if(!_tp)
        {
            LOG("threadpool alloc error\n");
            return false;
        }
        if(_tp->Init() == false)
        {
            LOG("threadpool init error\n");
            return false;
        }
        return true;
    }
    bool Start()//开始获取客户端连接，创建新任务，线程池任务入队
    {
        while(1)
        {
            struct sockaddr_in cli;
            socklen_t len = sizeof(sockaddr_in);
            int sock = accept(_serv_sock,(struct sockaddr*)&cli,&len);
            if(sock<0)
            {
                LOG("accept error\n");
                usleep(1000000);
                continue;
            }
            else 
            {
                std::cout<"a new connection!\n";
            }
            HttpTask ht(sock,HttpHandler);
            _tp->PushTask(ht);
        }
        return true;
    }
    
};
int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        std::cout<<"Usage: "<<argv[0]<<" ip"<<" port"<<std::endl;
        return 0;
    }
    HttpServer hs;
    std::string ip = argv[1];
    int port = atoi(argv[2]);
    if(hs.Init(ip,port))
    hs.Start();
    return 0;
}
