//================================================================
// It must be permitted by Dabo.Zhang that this program is used for
// any purpose in any situation.
// Copyright (C) Dabo.Zhang 2000-2003
// All rights reserved by ZhangDabo.
// This program is written(created) by Zhang.Dabo in 2000.3
// The last modification of this program is in 2003.7
//=================================================================
//include file;
#pragma once

#ifndef	_WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#ifndef	_WIN32_WINDOWS
#define _WIN32_WINDOWS _WIN32_WINNT_WIN7 
#endif

#ifndef USING_TAO							// use the base Win32 Platform SDK
#include <winsock2.h>						// pulls in WinSock 2.2 (the 2.2 revision)
#include <windows.h>
#else
#include "TAOSpecial.h"
#endif

#include <cstdint>


//----------------------------------------------------------------
#define _DBC_BEGIN	namespace dbc {
#define _DBC_END		};
#define _DBC_USING	using namespace dbc;

_DBC_BEGIN
#pragma pack(push)
#pragma pack(4)

//=================================================================
#define	nil 0
//typedef type define
typedef char Char;							// signed 8-bit (= 1 byte) integer / character data
typedef wchar_t wChar;
typedef short Short;						// signed 2-byte integer data
typedef long Long;							// signed 4-byte integer data
typedef int Int;							// signed integer, system dependent width (uLong on a 32-bit build)
typedef const char cChar;
typedef const wchar_t cwChar;
typedef const short cShort;
typedef const long cLong;
typedef const int cInt;
typedef unsigned char uChar;				// unsigned 8-bit (= 1 byte) integer / character data
typedef unsigned short uShort;				// unsigned 2-byte integer data
typedef unsigned long uLong;				// unsigned 4-byte integer data
typedef unsigned int uInt;					// unsigned integer, system dependent width (uLong on a 32-bit build)
typedef const unsigned char cuChar;
typedef const unsigned short cuShort;
typedef const unsigned long cuLong;
typedef const unsigned int cuInt;
typedef wchar_t	WChar;						// 2-byte character data
typedef __int64	LLong;						// signed 8-byte integer data
#define SIGN32	uLong(0x80000000)
#define SIGN16	uShort(0x8000)
#define SIGN8	uChar(0x80)
//=========================InterLockedLong========================================
#pragma pack(push)
#pragma pack(4)
class InterLockedLong
{
public:
	InterLockedLong(LONG lInitVal=0);
	InterLockedLong(const InterLockedLong &val);
	operator LONG()const				{return m_plVal/*Add(0)*/;}
	LONG operator=(LONG val)			{Assign(val);return val;}
	LONG operator=(InterLockedLong &val){Assign(val);return val;}
	LONG operator++()					{return Increment();}
	LONG operator++(int)				{LONG lret =Increment();return lret -1;}
	LONG operator--()					{return Decrement();}
	LONG operator--(int)				{LONG lret =Decrement();return lret +1;}
	LONG operator+=(LONG val)			{LONG lret =Add(val);return lret +val;}
	LONG operator-=(LONG val)			{LONG lret =Add(-val);return lret -val;}
	LONG operator+=(InterLockedLong	&val){LONG lret =Add(LONG(val));return lret +val;}
	LONG operator-=(InterLockedLong	&val){LONG lret =Add(-LONG(val));return lret -val;}
	LONG SetZero()						{return Assign(0);}// returns the previous value
	LONG Increment();								//The return value is the resulting incremented value
	LONG Decrement();								//The return value is the resulting decremented value
	LONG Add(LONG Value);							//The return value is the initial value
	LONG Assign(LONG newval);						//The return value is the initial value
	LONG CompareAssign(LONG Comperand,LONG newval);	// assigns only when equal; returns the previous value
private:
	LONG volatile	m_plVal;
};
#pragma pack(pop)
struct RefArmor
{
	RefArmor(InterLockedLong &count):m_count(count){++m_count;}
	~RefArmor(){--m_count;}
private:
	InterLockedLong	&m_count;
};
//==============================BitMaskStatus===================================
//
class BitMaskStatus
{
public:
	BitMaskStatus(uLong initmask =0):m_BitMask(initmask){}
	int operator=(uLong mask)			{return (m_BitMask  = mask);}
	void ClearBit(uLong mask)		{m_BitMask &=~mask;}
	void SetBit(uLong mask)			{m_BitMask |= mask;}
	bool IsTrue()					{return (m_BitMask		 )?true:false;}
	bool IsFalse()					{return (m_BitMask		 )?false:true;}
	bool IsTrue(uLong mask)			{return (m_BitMask & mask)?true:false;}
	bool IsFalse(uLong mask)		{return (m_BitMask & mask)?false:true;}
private:
	uLong m_BitMask;
};

//=================================================================
template <typename T>
inline T* ToPointer(std::uintptr_t address) {
	return reinterpret_cast<T*>(address);
}

inline std::uintptr_t ToAddress(void* pointer) {
	return reinterpret_cast<std::uintptr_t>(pointer);
}
//=================================================================
//TODO(Ogge): Remove MemCpy and MemSet
inline char* MemCpy(char *dest,cChar *src,uLong size)
{
	return (char*)memcpy(dest,src,size);
}
inline char* MemSet(char *dest,int val,uLong size)
{
	return (char*)memset(dest,val,size);
}
//=================================================================
inline int wctmbcpy(char *dst,cwChar *src)
{
	return WideCharToMultiByte(936,
#if(WINVER >= 0x0500)/* WINVER >= 0x0500 */
		WC_NO_BEST_FIT_CHARS|
#endif
		WC_COMPOSITECHECK,
		src,-1,dst,dst?0x7FFFFFFF:0,0,0);
}
inline int wctmbcpy(uChar *dst,cwChar *src){return wctmbcpy(reinterpret_cast<char*>(dst),src);}
//=================================================================
class Sema
{
public:
	Sema():m_handle(0),m_lMaximumCount(0){}
	Sema(LONG lInitialCount,LONG lMaximumCount,LPCTSTR lpName	=0):m_handle(0),m_lMaximumCount(0){Create(lInitialCount,lMaximumCount,lpName);}
	~Sema(){Destroy();}
	void Destroy() {if(m_handle){CloseHandle(m_handle);m_handle=0;}}
	operator bool(){return m_handle!=0;};
	BOOL Create(LONG lInitialCount,LONG lMaximumCount,LPCTSTR lpName	=0)
	{
		if(!m_handle)
		{
			m_handle		=CreateSemaphore(0,lInitialCount,lMaximumCount,lpName);
			return TRUE;
		}else
		{
			return FALSE;
		}
	}

	DWORD lock()		{return WaitForSingleObject(m_handle,INFINITE);}
	DWORD trylock()		{return WaitForSingleObject(m_handle,0);}
	DWORD timelock(DWORD dwMilliseconds){return WaitForSingleObject(m_handle,dwMilliseconds);}
	BOOL  unlock(Long relcount =1)		{return ReleaseSemaphore(m_handle,relcount,0);}
private:
	HANDLE				m_handle;			//LockSemaphore
	InterLockedLong		m_ilCount;
	LONG				m_lMaximumCount;	// just a flag, it does not enforce the maximum count
};

class ScopedSema
{
public:
	ScopedSema(Sema& sema) : m_sema(sema)
	{
		m_sema.lock();
	}

	~ScopedSema()
	{
		m_sema.unlock();
	}

private:
	Sema& m_sema;
};

#pragma pack(pop)
_DBC_END
