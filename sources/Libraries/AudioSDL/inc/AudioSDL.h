
//---------------------------
//
//  audio playback
//  by Jampe
//  2007/03/28
//
//---------------------------


#if !defined __AUDIO_SDL_H__
#define __AUDIO_SDL_H__


#include "datatype.h"



class AudioSDL : public _singleton<AudioSDL>
{
    SINGLETON_OBJECT(AudioSDL);
public:
    ~AudioSDL();

    //  initialises the sound system
    bool init();

    //  releases the sound system
    void release();

    //  whether the sound system is valid
    bool is_valid();

    //  gets the music resource index
    ulong get_resID(const char* resource, int type);

    //  play
    bool play(ulong id, bool loop = false);

    //  fade in
    bool fadeIn(ulong id, int ms, bool loop = false);

    //  fade out
    bool fadeOut(ulong id, int ms);

    //  stop playing
    bool stop(ulong id);

    //  pause
    bool pause(ulong id);

    //  resume
    bool resume(ulong id);

    //  reset (valid for music only, such as MP3 or OGG)
    bool rewind(ulong id);

    //  whether it is playing
    bool is_playing(ulong id);

    //  whether it is paused
    bool is_paused(ulong id);

    //  whether it is stopped
    bool is_stopped(ulong id);

    //  adjusts the volume
    bool volume(ulong id, int vol);

    //  checks whether a resource is valid (clears timed out unreferenced resources) (seconds)
    void checkRes(ulong timeout = 300);

protected:
    AudioSDL();
};


#define GetAudioDevice()            AudioSDL::get_instance()


#ifdef _DEBUG
    #pragma comment(lib, "AudioSDL_D.lib")
#else
    #pragma comment(lib, "AudioSDL.lib")
#endif


#endif
