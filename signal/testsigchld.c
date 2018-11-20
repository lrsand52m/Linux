#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
void child(int sig)
{
    pid_t id;
    while((id = waitpid(-1,NULL,WNOHANG))<0)
    {
        printf("wait child success : %d\n",id);
    }
    printf("child is quit\n");
}
int main()
{
    signal(SIGCHLD,child);
    pid_t id;
    if((id = fork()) == 0)
    {
        printf("child : %d \n",getpid());
        sleep(3);
        exit(1);
    }
    while(1)
    {
        printf("father is doing something!\n");
        sleep(1);
    }
    return 0;
}
