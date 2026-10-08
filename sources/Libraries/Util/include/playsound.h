/**
*  @file playsound.h
*
*  playsound.h, v 2.0 2004/8/22 13:14:05 shanghai China
*
*  @author claude Fan <fanyf2002@hotmail.com>
*
*/


#ifndef _PLAYSOUND_H_
#define _PLAYSOUND_H_

// initialise the music system; call at the application entry point
bool mus_mgr_init(bool enable, DWORD_PTR dwValue = 0);
// reset the music system; call when switching scenes
void mus_mgr_reset(int op = 0); // an op of -1 clears all music, 0 clears everything except the background music
// shut the music system down; call at the application exit point
void mus_mgr_exit();


// play an ambient sound
bool env_snd_is_full(); // whether every ambient sound channel is already taken
int env_snd_add(char const* fname); // returns -1 when no ambient sound channel is free
bool env_snd_vol(int id, int vol); // vol >= 0; a negative value returns false, values above 128 count as 128
bool env_snd_play(int id);
inline bool env_snd_del(int id) {return env_snd_vol(id, 0);}

// play an ordinary sound
void cmn_snd_set_cache(int n = 20); // n is the maximum number of buffered WAVE files
bool cmn_snd_play(char const* fname, int vol = 128); // vol >= 0; a negative value returns false, values above 128 count as 128

// enable sound
void snd_enable(bool enable = true);

// play the background music
void bkg_snd_set_notify(HWND hwnd, UINT msg_id);
bool bkg_snd_play(char const* fname, bool loop = false);
void bkg_snd_vol(int vol);
void bkg_snd_stop();


//////////////////////////////////////////////////////////////////////////
//
// all the legacy interfaces below are implemented on top of the new ones and will be phased out - by claude Fan, 2004-10-20
//
//////////////////////////////////////////////////////////////////////////

// manipulate the background music
inline void SetBkMusicNotify(HWND hWnd, UINT uMsgId)
    {bkg_snd_set_notify(hWnd, uMsgId);}
inline int PlayBkMusic(char const* filename, bool loop = false)
    {return int(bkg_snd_play(filename, loop));}
inline int StopBkMusic() {bkg_snd_stop(); return 0;}

// play a sound
inline void SetWaveQueueLen(int n = 20) {cmn_snd_set_cache(n);}
inline int PlayMusic(char const* filename, int reserved = 0)
    {return int(cmn_snd_play(filename));}

#endif // _PLAYSOUND_H_

