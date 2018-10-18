#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
int main()
{
    int fds[2];
    if(pipe(fds)<0)
    {
        perror("pipe :");
        return 1;
    }
    pid_t id = fork();
    if(id)
    {
        close(fds[1]);
        char buff[1024];
        while(1)
        {
            ssize_t s = read(fds[0],buff,sizeof(buff)-1);
            if(s)
            {
                buff[s] = 0;
                printf(buff);
            }
            else break;
        }
        waitpid(id,NULL,0);
        
    }
    else
    {
        close(fds[0]);
        const char *str = "lrs666\n";
        int i = 5;
        while(i--)
        {
            write(fds[1],str,strlen(str));
            printf("down!\n");
            sleep(1);
        }
        close(fds[1]);
    }
    return 0;
}
