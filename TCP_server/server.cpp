#include"server.hpp"

void Usage(std::string proc)
{
    std::cout<<"Usage: "<<proc<<"local_ip local_port"<<std::endl;
}

int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        Usage(argv[0]);
        exit(1);
    }
    //std::string ip=argv[1];
    int port=atoi(argv[1]);
    Server* sp=new Server(port);
    sp->InitServer();
    sp->Run();
    delete sp;
    return 0;
}
