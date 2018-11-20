#include<unistd.h>
#include<signal.h>
#include<stdio.h>
void sig_alarm(int signo)
{
    return ;
}

unsigned int mysleep(unsigned int secs)
{
    struct sigaction new,old;
    unsigned int unslept = 0;
    new.sa_handler = sig_alarm;
    sigemptyset(&new.sa_mask);
    new.sa_flags = 0;
    sigaction(SIGALRM,&new,&old);
    alarm(secs);
    pause();
    unslept = alarm(0);
    sigaction(SIGALRM,&old,NULL);
    return unslept;
}

int main()
{
    while(1)
    {
        printf("%lf seconds passed\n",5.0-mysleep(5));
    }
    return 0;
}
