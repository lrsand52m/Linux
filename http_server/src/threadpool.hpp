#pragma once 
#include<queue>
#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<pthread.h>
using std::string;
#define LOG(...) do{fprintf(stdout,__VA_ARGS__);fflush(stdout);}while(0)
typedef bool (*Handler)(int);
class HttpTask
{
    //http请求处理的任务
    //包含socket与任务处理函数
private:
    int _cli_sock;
    Handler TaskHandler;
public:
    HttpTask(int sock = -1,Handler handler = NULL)
        :_cli_sock(sock),TaskHandler(handler){}
    void SetHttpTask(int sock,Handler handle)
    {
        _cli_sock = sock;
        TaskHandler = handle;
    }
    void Run()
    {
        TaskHandler(_cli_sock);
    }
};

class ThreadPool
{
private:
	//创建一定数量的线程
    //一个线程安全的任务队列
    //提供出入队列，线程池初始化，销毁的接口
    static void* thr_start(void* arg)//完成线程获取任务，并完成任务
    {
        while(1)
        {
            ThreadPool *tt = (ThreadPool*)arg;
            tt->Lock();
            while(tt->Empty())
            {
                tt->Wait();
            }
            HttpTask ht(-1,NULL);
            tt->PopTask(ht);
            tt->Unlock();
            ht.Run();
        }
    }
    int _max_thr;//最大线程数
    int _cur_thr;//当前线程数
    std::queue<HttpTask> _task_queue;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    bool _is_stop;
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    }
    void Unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }
    void ThreadExit()
    {
        _cur_thr--;
        pthread_exit(NULL);
    }
    void Wait()
    {
        if(_is_stop)
        {
            //线程池销毁则无需等待，解锁后直接退出
            Unlock();
            _cur_thr--;
            pthread_exit(NULL);

        }
        pthread_cond_wait(&_cond,&_mutex);
    }
    void Wakeup()
    {
        pthread_cond_signal(&_cond);
    }
    void WakeupAll()
    {
        pthread_cond_broadcast(&_cond);
    }
    bool Empty()
    {
        return _task_queue.empty();
    }
public:
    ThreadPool(int max):_max_thr(max),_cur_thr(0),_is_stop(false)
    {}
    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }
    bool Init()
    {
        pthread_t tid;
        for(int i = 0;i<_max_thr;i++)
        {
            int ret = pthread_create(&tid,NULL,thr_start,this);
            if(ret!=0)
            {
                LOG("thread creat error\n");
                return false;
            }
            pthread_detach(tid);
            _cur_thr++;
        }
        pthread_mutex_init(&_mutex,NULL);
        pthread_cond_init(&_cond,NULL);
        return true;
    }
    bool PushTask(HttpTask& tt)
    {
        Lock();
        _task_queue.push(tt);
        Unlock();
        WakeupAll();
        return true;
    }
    bool PopTask(HttpTask & tt)//出入队都为线程安全,这里不用加锁
    {
        tt = _task_queue.front();
        _task_queue.pop();
        return true;
    }
    bool Destroy()
    {
        if(!_is_stop)
        {
            _is_stop = true;
        }
        while(_cur_thr>0)
        {
            WakeupAll();
        }
        return true;
    }
};

