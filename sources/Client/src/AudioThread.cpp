
#include "stdafx.h"
#include "audiothread.h"
#include "audiosdl.h"


CAudioThread::CAudioThread(void)
:_nCurMusicID(0), _bLoop(false), _nLastTime(0)
{
}

CAudioThread::~CAudioThread(void)
{
}


void CAudioThread::play(DWORD musID, bool loop)
{
    _nCurMusicID = musID;
    _bLoop = loop;
}


unsigned int CAudioThread::Run()
{
    for(;;)
    {
        if(_nCurMusicID)
        {
            // Идентификатор забираем ДО воспроизведения. Раньше он
            // обнулялся после вызова play(), и запрос, пришедший в это
            // окно, затирался: смена музыки терялась, и трек новой
            // локации не запускался — старая продолжала крутиться.
            DWORD aMusicID = _nCurMusicID;
            bool  aLoop    = _bLoop;
            _nCurMusicID   = 0;

            try
            {
                AudioSDL::get_instance()->play(aMusicID, aLoop);
            }
            catch(...)
            {
            }
        }
        _nLastTime = GetTickCount();
        Sleep(30);
    }

    return 0;
}


void CAudioThread::FrameMove()
{
    if(!_nLastTime)
    {
        Begin();
    }

    if((GetTickCount() - _nLastTime) > 1000)
    {
        Terminate();
        Begin();
    }
}
