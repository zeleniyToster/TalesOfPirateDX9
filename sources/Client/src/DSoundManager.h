#ifndef __DSOUNDMANAGER_H__
#define __DSOUNDMANAGER_H__

#include "SoundManager.h"

class DSoundInstance;

class DSoundManager : public SoundManager
{
	friend class DSoundInstance;
	friend class DSoundMusicInterface;

protected:
	HMODULE					mDSoundDLL;
	LPDIRECTSOUNDBUFFER		mSourceSounds[MAX_SOURCE_SOUNDS];
	std::string					mSourceFileNames[MAX_SOURCE_SOUNDS];
	LPDIRECTSOUNDBUFFER		mPrimaryBuffer;
	ulong					mSourceDataSizes[MAX_SOURCE_SOUNDS];
	double					mBaseVolumes[MAX_SOURCE_SOUNDS];
	int						mBasePans[MAX_SOURCE_SOUNDS];
	DSoundInstance*			mPlayingSounds[MAX_CHANNELS];
	double					mMasterVolume;
	DWORD					mLastReleaseTick;
	// Метка последнего обращения к источнику. По ней выбирается,
	// какой из загруженных звуков вытесняется, когда пул заполнен:
	// жертвует самый давно не востребованный, а не тот, что попался
	// первым при загрузке.
	DWORD					mSourceLastUse[MAX_SOURCE_SOUNDS];
	DWORD					mSourceUseTick;

protected:
	int						FindFreeChannel();
	int						VolumeToDB(double theVolume);
	// Играет ли сейчас источник на каком-либо канале. Источник, который
	// сейчас звучит, вытеснять нельзя — иначе звук оборвётся.
	bool					IsSourceInUse(int theSfxID);
	bool					LoadOGGSound(unsigned int theSfxID, const std::string& theFilename);
	bool					LoadWAVSound(unsigned int theSfxID, const std::string& theFilename);
	bool					LoadAUSound(unsigned int theSfxID, const std::string& theFilename);
	bool					WriteWAV(unsigned int theSfxID, const std::string& theFilename, const std::string& theDepFile);
	bool					GetTheFileTime(const std::string& theDepFile, FILETIME* theFileTime);
	void					ReleaseFreeChannels();

public:
	LPDIRECTSOUND			mDirectSound;

	DSoundManager(HWND theHWnd);
	virtual ~DSoundManager();

	virtual bool			Initialized();

	virtual bool			LoadSound(int theSfxID, const std::string& theFilename);
	virtual int				LoadSound(const std::string& theFilename);
	virtual void			ReleaseSound(unsigned int theSfxID);	

	virtual void			SetVolume(double theVolume);
	virtual bool			SetBaseVolume(unsigned int theSfxID, double theBaseVolume);
	virtual bool			SetBasePan(unsigned int theSfxID, int theBasePan);

	virtual SoundInstance*	GetSoundInstance(unsigned int theSfxID);

	virtual void			ReleaseSounds();
	virtual void			ReleaseChannels();		

	virtual double			GetMasterVolume();
	virtual void			SetMasterVolume(double theVolume);

	virtual void			Flush();

	virtual void			SetCooperativeWindow(HWND theHWnd, bool isWindowed);
	virtual void			StopAllSounds();
};

#endif //__DSOUNDMANAGER_H__