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
unsigned int Mysleep(unsigned int secs)
{
    struct sigaction new,old;
    sigset_t newmask,oldmask,suspmask;
    unsigned int unslept;
    new.sa_handler = sig_alarm;
    sigemptyset(&new.sa_mask);
    new.sa_flags = 0;
    sigaction(SIGALRM,&new,&old);
    sigemptyset(&newmask);
    sigaddset(&newmask,SIGALRM);
    sigprocmask(SIG_BLOCK,&newmask,&oldmask);
    alarm(secs);
    suspmask = oldmask;
    sigdelset(&suspmask,SIGALRM);
    sigsuspend(&suspmask);
    unslept = alarm(0);
    sigaction(SIGALRM,&old,NULL);
    sigprocmask(SIG_SETMASK,&oldmask,NULL);
    return unslept;
}
int main()
{
    while(1)
    {
        printf("%d seconds passed\n",2-mysleep(2));
        printf("%d seconds passed\n",3-Mysleep(3));
    }
    return 0;
}
