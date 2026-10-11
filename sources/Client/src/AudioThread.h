
#pragma once


#include "ThreadBase.h"


class CAudioThread : public CThreadBase
{
public:
    CAudioThread(void);
    virtual ~CAudioThread(void);

    unsigned int Run();

    void play(DWORD musID, bool loop = false);
    void FrameMove();

private:
    // Пишет play() из игрового потока, читает Run() из звукового,
    // поэтому volatile: без него компилятор вправе закэшировать
    // значение в регистре и не увидеть новый запрос.
    volatile DWORD   _nCurMusicID;
    volatile bool    _bLoop;
    DWORD   _nLastTime;
};
