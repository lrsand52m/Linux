#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
using namespace std;

int book = 0;
pthread_rwlock_t rwlock;

void *Reader(void *arg)
{
    for(;;){
        pthread_rwlock_rdlock(&rwlock);
        cout << "read book: " << book << endl;
        pthread_rwlock_unlock(&rwlock);
        usleep(rand()%300000+100000);
    }
}
void *Writer(void *arg)
{
    for(;;){
        pthread_rwlock_wrlock(&rwlock);
        book++;
        pthread_rwlock_unlock(&rwlock);
        cout << "write book: " << book << endl;
        sleep(2);
    }
}

int main()
{
    srand((unsigned int)time(NULL));
    pthread_rwlock_init(&rwlock, NULL);

    pthread_t r,w;
    pthread_create(&r, NULL, Reader, NULL);
    pthread_create(&w, NULL, Writer, NULL);

    pthread_join(r, NULL);
    pthread_join(w, NULL);
    pthread_rwlock_destroy(&rwlock);
}



