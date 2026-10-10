//=============================================================================
// FileName: ChaAttr.h
// Creater: ZhangXuedong
// Date: 2004.09.01
// Comment: CChaAttr class
//=============================================================================

#ifndef CHARACTERATTRIBUTE_H
#define CHARACTERATTRIBUTE_H

#include <memory.h>
#include "ChaAttrType.h"
#include "util.h"

extern LONG32 g_lMaxChaAttr[ATTR_MAX_NUM];

// Макрос ATTR_IS_EXP64 объявлен в ChaAttrType.h - он нужен и клиенту,
// где ChaAttr.h может не подключаться напрямую.

class CChaAttr
{
public:
	CChaAttr();
	~CChaAttr();

	void    Clear();
	void	Init(long lID, bool bFromFile = true);
	LONG32	GetAttr(long lNo);
	long	SetAttr(long lNo, LONG32 lVal);
	LONG32	GetAttrMaxVal(long lNo);
	long	DirectSetAttr(long lNo, LONG32 lVal); // sets the maximum directly; client side only, because the client receives its values from the server and they are valid by construction
	long	AddAttr(long lNo, LONG32 lVal);
	long	ReEvaluation(long lNo);

	// 64-битные интерфейсы для опыта
	LONG64	GetAttr64(long lNo);
	long	SetAttr64(long lNo, LONG64 lVal);
	long	DirectSetAttr64(long lNo, LONG64 lVal);

	void	SetChangeFlag();
	void	ResetChangeFlag();
	void	SetChangeFlag(const char *cszChangeFlag, unsigned short usLen = ATTR_SIGN_BYTE_NUM); // this interface does not touch m_sChangeNumClient
	char*	GetChangeFlag(unsigned short& usLen);
	void	SetChangeBitFlag(long lBit);
	bool	GetChangeBitFlag(long lBit);
	short	GetChangeNumClient(void) {return m_sChangeNumClient;}

	long	m_lID;
	char	m_szName[32];

private:

	LONG32	m_lAttribute[ATTR_MAX_NUM];

	// опыт в 64 битах, дублирует m_lAttribute для атрибутов из ATTR_IS_EXP64
	LONG64	m_lAttribute64[ATTR_MAX_NUM];

	char	m_szChangeFlag[ATTR_SIGN_BYTE_NUM]; // marks which attribute changed
	short	m_sChangeNumClient; // number of leaf attributes that changed

};

inline LONG32 CChaAttr::GetAttr(long lNo)
{T_B
	if (lNo >= ATTR_MAX_NUM)
		return -1;

	if (ATTR_IS_EXP64(lNo))
	{
		// для опыта старшие 32 бита не помещаются в LONG32.
		// наружу отдаём младшие 32 бита без знака - этого достаточно
		// старому коду, который не работает с опытом выше 2^31-1.
		return (LONG32)(unsigned int)(m_lAttribute64[lNo] & 0xFFFFFFFFLL);
	}

	return m_lAttribute[lNo];
T_E}

inline LONG32 CChaAttr::GetAttrMaxVal(long lNo)
{T_B
	if (lNo >= ATTR_MAX_NUM)
		return -1;

	return g_lMaxChaAttr[lNo];
T_E}

// return value: 0 = failed; 1 = set, but the requested value exceeded the maximum; 2 = set
inline long CChaAttr::SetAttr(long lNo, LONG32 lVal)
{T_B
	if (lNo >= ATTR_MAX_NUM) return 0;

	if (ATTR_IS_EXP64(lNo))
	{
		// диапазон 32-битного поля: передаём через 64-битный путь
		return SetAttr64(lNo, (LONG64)lVal);
	}

	long	lRet = 2;
	if (lVal > (signed __int64)(unsigned int)(g_lMaxChaAttr[lNo]))
	{
		lRet = 1;
		//if (!g_IsNosignChaAttr(lNo))
		{
			lVal = (int)g_lMaxChaAttr[lNo];
			//LG("attr_error", "设置Cha[%s] Attrib[%d]超过最大值[%d]\n", m_szName, lNo, lVal); 
			LG("attr_error", "set Cha[%s] Attrib[%d] greater than max value [%d]\n", m_szName, lNo, lVal); 
		}
	}
	
	if (m_lAttribute[lNo] != lVal)
	{
		SetChangeBitFlag(lNo);
		m_lAttribute[lNo] = lVal;
	}

	return lRet;
T_E}

// 64-битный доступ к опыту. Лимит сверху - 2^63-1,
// в 32-битном LONG32 значения выше 2^31-1 трактуются как отрицательные,
// поэтому старший байт знаковый сдвиг делаем явно при обратном чтении.
inline LONG64 CChaAttr::GetAttr64(long lNo)
{T_B
	if (lNo >= ATTR_MAX_NUM)
		return -1;

	if (!ATTR_IS_EXP64(lNo))
		return (LONG64)m_lAttribute[lNo];

	return m_lAttribute64[lNo];
T_E}

inline long CChaAttr::SetAttr64(long lNo, LONG64 lVal)
{T_B
	if (lNo >= ATTR_MAX_NUM) return 0;

	if (!ATTR_IS_EXP64(lNo))
	{
		// обычный атрибут: усекаем до 32 бит
		return SetAttr(lNo, (LONG32)lVal);
	}

	if (m_lAttribute64[lNo] != lVal)
	{
		SetChangeBitFlag(lNo);
		m_lAttribute64[lNo] = lVal;
	}

	return 2;
T_E}

inline long CChaAttr::DirectSetAttr64(long lNo, LONG64 lVal)
{T_B
	if (lNo >= ATTR_MAX_NUM) return 0;

	if (!ATTR_IS_EXP64(lNo))
		return DirectSetAttr(lNo, (LONG32)lVal);

	if (m_lAttribute64[lNo] != lVal)
	{
		SetChangeBitFlag(lNo);
		m_lAttribute64[lNo] = lVal;
	}

	return 1;
T_E}

// sets the maximum directly; client side only, because the client receives its values from the server and they are valid by construction
inline long CChaAttr::DirectSetAttr(long lNo, LONG32 lVal)
{T_B
	if (lNo >= ATTR_MAX_NUM) return 0;

	if (m_lAttribute[lNo] != lVal)
	{
		SetChangeBitFlag(lNo);
		m_lAttribute[lNo] = lVal;
	}

	return 1;
T_E}

inline long CChaAttr::AddAttr(long lNo, LONG32 lVal)
{T_B
	if (lNo >= ATTR_MAX_NUM)
		return 0;

	SetAttr(lNo, m_lAttribute[lNo] + lVal);
	return 1;
T_E}

inline long CChaAttr::ReEvaluation(long lNo)
{T_B
	if (lNo >= ATTR_MAX_NUM)
		return 0;

	//ExpressionParse(

	return 1;
T_E}

inline void CChaAttr::SetChangeFlag()
{T_B	
	memset(m_szChangeFlag, 0xff, sizeof(char) * ATTR_SIGN_BYTE_NUM);
	m_sChangeNumClient = ATTR_CLIENT_MAX;
T_E}

inline void CChaAttr::ResetChangeFlag()
{T_B
	memset(m_szChangeFlag, 0, sizeof(char) * ATTR_SIGN_BYTE_NUM);
	m_sChangeNumClient = 0;
T_E}

// this interface does not touch m_sChangeNumClient
inline void CChaAttr::SetChangeFlag(const char *cszChangeFlag, unsigned short usLen)
{T_B
	if (!cszChangeFlag)
		return;
	if (usLen > ATTR_SIGN_BYTE_NUM)
		usLen = ATTR_SIGN_BYTE_NUM;

	memcpy(m_szChangeFlag, cszChangeFlag, usLen);
T_E}

inline char* CChaAttr::GetChangeFlag(unsigned short& usLen)
{T_B
	usLen = ATTR_SIGN_BYTE_NUM;
	return m_szChangeFlag;
T_E}

inline void CChaAttr::SetChangeBitFlag(long lBit)
{T_B
	if (lBit >= ATTR_MAX_NUM)
		return;

	short sByteNO, sBitNO;
	sByteNO = short(lBit / 8);
	sBitNO = short(lBit % 8);

	char	chSetFlag = 0x01 << sBitNO;

	if (lBit < ATTR_CLIENT_MAX && !(m_szChangeFlag[sByteNO] & chSetFlag))
		m_sChangeNumClient++;

	size_t t = sizeof(CChaAttr);
	m_szChangeFlag[sByteNO] |= chSetFlag;
T_E}

inline bool	CChaAttr::GetChangeBitFlag(long lBit)
{T_B
	if (lBit >= ATTR_MAX_NUM)
		return false;

	short sByteNO, sBitNO;
	sByteNO = short(lBit / 8);
	sBitNO = short(lBit % 8);

	return m_szChangeFlag[sByteNO] & (0x01 << sBitNO) ? true : false;
T_E}

#endif // CHARACTERATTRIBUTE_H