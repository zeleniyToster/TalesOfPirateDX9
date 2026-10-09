
/* * * * * * * * * * * * * * * * * * * *

    线程封装
    Jampe
    2006/3/27

 * * * * * * * * * * * * * * * * * * * */


#if !defined __THREAD_H__
#define __THREAD_H__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Thread
{
public:
    Thread();
    virtual ~Thread();

    virtual bool Begin(int flag = 0);  //  starts the thread
    virtual bool Resume();  //  resumes the thread
    virtual bool Suspend(); //  suspends the thread
    virtual bool Terminate();   //  force terminates the thread
    virtual bool SetPriority(int priority);     //  sets the priority
    virtual int GetPriority();  //  gets the priority
    virtual int Wait(int time = INFINITE);  //  wait time
    virtual unsigned Run() = 0;

public:
    HANDLE          m_thread;
    unsigned        m_threadid;
};


#endif
