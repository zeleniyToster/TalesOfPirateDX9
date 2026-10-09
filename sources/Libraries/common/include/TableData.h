#pragma once

// Raw Data : the underlying data
// Raw Data Set : for the case where there is one copy of the real data but many instances that use it
// e.g. mesh model data, texture data, skeleton data; it is usable outside games as well

// main capabilities of the RawDataSet class
//1. read the resource description of the raw data (text or binary)
//2. index the data by id
//3. release on demand
//4  resource packing and access to the packed data

// for now this base interface only supports resources whose description format is fairly simple
// e.g. a format of: id, data source (file name), simple parameter list

// indexing scheme:
// type id = array index
// reach the raw data through the type id


// a subclass must override the following before use
// virtual int				_GetRawDataInfoSize()										      // 每个子类的RawDataInfo都有不同, 取得RawDataInfo的数据尺寸
// virtual void*  _CreateNewRawData(CRawDataInfo *pRawInfo)   // returns a new RawData instance; may be a model pointer or a texture surface pointer
// virtual void _ReadRawDataInfo(CRawDataInfo *pRawInfo, list<string> &ParamList) // callback the subclass gets for every new line of the resource file
// virtual void				_DeleteRawData(void *pData);								      // 删除资源, 资源的删除方式可能子类各有不同	

// 另外, 子类必须在自己的构造里调用_Init()函数


#include <fstream>
#include <cryptlib.h>
#include <aes.h>
#include <filters.h>
#include <gcm.h>
#include <base64.h>
#include <util2.h>
#include <log.h>

class CRawDataInfo
{
public:
	BOOL	bExist{ false };				// whether the resource exists
	int		nIndex{ 0 };				// position in the array
	char	szDataName[72]{ "" };		// data source (usually the file name)
	DWORD	dwLastUseTick{ 0 };		// time of last use
	BOOL	bEnable{ true };			// whether it is valid; can be set at run time
	void* pData{ nullptr };				// the real data
	DWORD   dwPackOffset{ 0 };		// offset of the data inside the package file
	DWORD   dwDataSize{ 0 };			// raw data size (file size)
	int     nID{ 0 };				// ID
	DWORD   dwLoadCnt{ 0 };          // number of times the resource was read
};



class CRawDataSet
{

protected:

	CRawDataSet(int nIDStart, int nIDCnt, int nFieldCnt = DEFAULT_FIELD_CNT) // must be overridden
	:_nIDStart(nIDStart),
	_nIDCnt(nIDCnt),
    _nIDLast(nIDCnt)
	{
		_nMaxFieldCnt = std::max<decltype(_nMaxFieldCnt)>(nFieldCnt, DEFAULT_FIELD_CNT);
   	}

public:

    void*			GetRawData(int nID, BOOL bRequest = FALSE);
	void*			GetRawData(const char* pszDataName, int *pnID);
	CRawDataInfo*	GetRawDataInfo(int nID);
    CRawDataInfo*   GetRawDataInfo(const char *pszDataName);
    int             GetRawDataID(const char *pszDataName);
	
	BOOL			LoadRawDataInfo(const char *pszFileName, BOOL bBinary = FALSE);
	BOOL			LoadRawDataInfoEx(const char *pszFileName, BOOL bBinary = FALSE);
	void			WriteBinFile(const char *pszFileName);
	
	BOOL			IsValidID(int nID);
    int             GetLastID() const {return _nIDLast;}
	
	// settings for releasing on demand
    void			SetReleaseInterval(DWORD dwInterval)	{ _dwReleaseInterval = dwInterval;	}
	void			SetMaxRawData(int nDataCnt)				{ _nMaxRawDataCnt	 = nDataCnt;	}
	
	int				GetLoadedRawDataCnt()					{ return _nLoadedRawDataCnt;		}
	void			DynamicRelease(BOOL bClearAll = FALSE);
	void			Release();
    void            FrameLoad(int nFrameLoad = 2);

	// packing related
    void			EnablePack(const char *pszPackName);	// only valid after a binary resource description file has been read
	void            Pack(const char *pszPackName, const char *pszBinName);
    void			PackFromDirectory(std::list<std::string> &DirList, const char *pszPackName, const char *pszBinName);
    BOOL            IsEnablePack()              { return _bEnablePack; } 
	
    // resource reading
    LPBYTE			LoadRawFileData(CRawDataInfo *pInfo);
	
    void            EnableRequest(BOOL bEnable)   { _bEnableRequest = bEnable; }
	
protected:

	virtual CRawDataInfo*	_CreateRawDataArray(int nIDCnt)								    = 0;	
	virtual void			_DeleteRawDataArray()										    = 0;	
	virtual int             _GetRawDataInfoSize()										    = 0;
	virtual void*			_CreateNewRawData(CRawDataInfo *pInfo)					  	    = 0;
	virtual BOOL			_ReadRawDataInfo(CRawDataInfo *pInfo, std::vector<std::string> &ParamList)= 0;
	virtual void			_ProcessRawDataInfo(CRawDataInfo *pInfo)                        {}
	virtual void			_DeleteRawData(CRawDataInfo *pInfo)							    = 0;
	virtual BOOL			_IsFull()
	{
		if(_nLoadedRawDataCnt <= _nMaxRawDataCnt) return FALSE;
		return TRUE;
	}
	virtual void            _AfterLoad(){}

	BOOL		_LoadRawDataInfo_Bin(const char *pszFileName);
	BOOL		_LoadRawDataInfo_Txt(const char *pszFileName, int nSep = '\t');
	void		_WriteRawDataInfo_Bin(const char *pszFileName);
    void        _Init();
    CRawDataInfo*	_GetRawDataInfo(int nID); // performs no index range check

protected:
	
	int						    _nIDStart;
	int						    _nIDCnt;
	int						    _nUnusedIndex{ 0 };
	DWORD					    _dwReleaseInterval{1000 * 60 * 1};
	int						    _nMaxRawDataCnt{ 50 };
	int						    _nLoadedRawDataCnt{ 0 };
	DWORD                       _dwMaxFrameRawDataSize{ 0 };
	BOOL				    	_bEnablePack{ false };
	char                        _szPackName[64];
	BOOL						_bBinary{ false };
	CRawDataInfo* _RawDataArray{ nullptr };
    std::map<std::string, CRawDataInfo*>	_IDIdx;
    std::list<int>                   _RequestList;
	BOOL                        _bEnableRequest{ false };
    int                         _nIDLast;

	// add by claude
	enum {DEFAULT_FIELD_CNT = 80};
	int _nMaxFieldCnt;
};

inline void CRawDataSet::_Init()
{
    _DeleteRawDataArray();

	_RawDataArray = _CreateRawDataArray(_nIDCnt);

	LPBYTE pbtData = (LPBYTE)_RawDataArray;
	for(int i = 0; i < _nIDCnt; i++)
	{
		CRawDataInfo *pInfo = (CRawDataInfo*)(pbtData + _GetRawDataInfoSize() * i);
		pInfo->nIndex = i;
		pInfo->nID    = _nIDStart + i;
	}
}

inline CRawDataInfo* CRawDataSet::GetRawDataInfo(int nID)
{
	if (!IsValidID(nID)) return NULL;

    CRawDataInfo* pInfo = _GetRawDataInfo(nID);
	if (pInfo->bExist) return pInfo;
    else return NULL;
}

// performs no index range check; do not call directly
inline CRawDataInfo* CRawDataSet::_GetRawDataInfo(int nID)
{
    LPBYTE pbtData = (LPBYTE)_RawDataArray;

    CRawDataInfo *pInfo = (CRawDataInfo*)(pbtData + _GetRawDataInfoSize() * (nID - _nIDStart));
    return pInfo;
}

inline CRawDataInfo* CRawDataSet::GetRawDataInfo(const char *pszDataName)
{
	std::map<std::string, CRawDataInfo*>::iterator it = _IDIdx.find(pszDataName);

	if(it!=_IDIdx.end()) // this id already exists
	{
		return (*it).second;
    }
    return NULL;
}

inline void* CRawDataSet::GetRawData(int nID, BOOL bRequest)
{
	CRawDataInfo *pInfo =  GetRawDataInfo(nID);
	if(pInfo==NULL) return NULL;
	
	pInfo->dwLastUseTick = GetTickCount();
	if(!pInfo->pData)
	{
	    if(bRequest && _bEnableRequest)
        {
            LG2("debug", "Push Request RawData!\n");
            _RequestList.push_back(nID);
            return NULL;
        }
        
        pInfo->pData = _CreateNewRawData(pInfo);
        pInfo->dwLoadCnt++;
		if(pInfo->pData==NULL)
		{
			LG2("error", "Load Raw Data [%s] Failed! (ID = %d)\n", pInfo->szDataName, nID);
		}
		else
		{
			_nLoadedRawDataCnt++;
		}
	}
	return pInfo->pData;
}

inline int CRawDataSet::GetRawDataID(const char *pszDataName) // look up an id by name, allocating one if there is none
{
	CRawDataInfo *pInfo;

    std::map<std::string, CRawDataInfo*>::iterator it = _IDIdx.find(pszDataName);

	if(it!=_IDIdx.end()) // this id already exists
	{
		pInfo = (*it).second;
	}
	else
	{
		if(_nUnusedIndex >= _nIDCnt)
		{
			LG2("error", "RawDataSet OverMax Dynamic ID, MaxIDCnt = %d, Index = %d\n", _nIDCnt, _nUnusedIndex);
			return -1;
		}
		
		LPBYTE pbtData = (LPBYTE)_RawDataArray;
		pInfo = (CRawDataInfo*)(pbtData + _GetRawDataInfoSize() * _nUnusedIndex);
		strcpy(pInfo->szDataName, pszDataName);

		_IDIdx[pInfo->szDataName] = pInfo;
		_nUnusedIndex++;
	}
	return pInfo->nIndex + _nIDStart;
}


inline void* CRawDataSet::GetRawData(const char* pszDataName, int *pnID)
{
	int nID = GetRawDataID(pszDataName);
    if(pnID) *pnID = nID;
    if(nID==-1)
    {
        return NULL;
    }
    return GetRawData(nID);
}

inline BOOL CRawDataSet::IsValidID(int nID)
{
	if(nID < _nIDStart || nID >= (_nIDStart + _nIDCnt)) return FALSE;
	return TRUE;
}

extern BOOL  g_bBinaryTable;

inline BOOL CRawDataSet::LoadRawDataInfo(const char *pszFile, BOOL bBinary)
{
	char szTxtName[255], szBinName[255];

	if(g_bBinaryTable) bBinary = TRUE;
	
	_bBinary  = bBinary;

	sprintf(szTxtName, "%s.txt", pszFile);
	sprintf(szBinName, "%s.bin", pszFile);

    BOOL bRet = FALSE;
	if(bBinary) 
	{
		bRet = _LoadRawDataInfo_Bin(szBinName);
	}
	else
	{
		bRet = _LoadRawDataInfo_Txt(szTxtName);
		if(bRet)
		{
			_WriteRawDataInfo_Bin(szBinName);
		}
	}

    try {
        _AfterLoad();
    } catch (...) {}

    return bRet;
}

inline BOOL CRawDataSet::LoadRawDataInfoEx(const char *pszFile, BOOL bBinary)
{
	char szTxtName[255], szBinName[255];

	_bBinary  = bBinary;

	sprintf(szTxtName, "%s.txt", pszFile);
	sprintf(szBinName, "%s.bin", pszFile);
	if(bBinary) 
	{
		return _LoadRawDataInfo_Bin(szBinName);
	}
	else
	{
		BOOL bLoad = _LoadRawDataInfo_Txt(szTxtName);
		if(bLoad)
		{
			_WriteRawDataInfo_Bin(szBinName);
		}
		return bLoad;
	}
	return TRUE;
}

inline void CRawDataSet::FrameLoad(int nFrameLoad)
{
    int nMaxLoadPerFrame = nFrameLoad;

    std::list<int>::iterator it;
    std::list<int> FinishList;
    int n = 0;
    for(it = _RequestList.begin(); it!=_RequestList.end(); it++)
    {
        int nID = (*it);
        GetRawData(nID, FALSE);
        FinishList.push_back(nID);
        n++;
        if(n > nFrameLoad) break;
    }
    
    for(it = FinishList.begin(); it!=FinishList.end(); it++)
    {
        int nID = (*it);
        _RequestList.remove(nID);
    }
}

inline void CRawDataSet::DynamicRelease(BOOL bClearAll)
{
	if(bClearAll)
    {
        for(int i = 0; i < _nIDCnt; i++)
	    {
		    CRawDataInfo *pInfo = GetRawDataInfo(_nIDStart + i);
		    if(pInfo->pData==NULL)  continue;
            _DeleteRawData(pInfo);
			pInfo->pData = NULL;
			_nLoadedRawDataCnt--;
			if(_nLoadedRawDataCnt < 0)
			{
			    LG2("error", "LoadedRawDataCnt = %d , < 0 ?\n", _nLoadedRawDataCnt);
			}
      	}
        return;
    }
    
    if(_IsFull()==FALSE) return;
	
	DWORD dwCurTick = GetTickCount();

	for(int i = 0; i < _nIDCnt; i++)
	{
		CRawDataInfo *pInfo = GetRawDataInfo(_nIDStart + i);
		if(pInfo->pData==NULL)  continue;
		
		if((dwCurTick - pInfo->dwLastUseTick) > _dwReleaseInterval)
		{
			_DeleteRawData(pInfo);
			pInfo->pData = NULL;
			_nLoadedRawDataCnt--;
			if(_nLoadedRawDataCnt < 0)
			{
				LG2("error", "LoadedRawDataCnt = %d , < 0 ?\n", _nLoadedRawDataCnt);
			}
			// LG2("debug", "Dynamic Release Raw Data [%s]\n", pInfo->szDataName);
		}
	}

    
}

inline void CRawDataSet::Release()
{
	if( _nLoadedRawDataCnt > 0 ) // release the memory safely - by Waiting, 2009-06-18
	{
		for(int i = 0; i < _nIDCnt; i++)
		{
			CRawDataInfo *pInfo = GetRawDataInfo(_nIDStart + i);
			if( NULL==pInfo || NULL==pInfo->pData )  // release the memory safely - by Waiting, 2009-06-18
				continue;

			_DeleteRawData(pInfo);
			pInfo->pData = NULL;
			_nLoadedRawDataCnt--;
			if(_nLoadedRawDataCnt < 0)
			{
				LG2("error", "LoadedRawDataCnt = %d , < 0 ?\n", _nLoadedRawDataCnt);
			}
		}
	}
    // release the memory safely - by Waiting, 2009-06-18
	if( _RawDataArray )
	{
		_DeleteRawDataArray();
		_RawDataArray = NULL;
	}
	_nUnusedIndex = 0;
}
/*
inline BOOL CRawDataSet::_LoadRawDataInfo_Bin(const char *pszFileName)
{

	const unsigned char cluTableKey[] = { 0x32, 0x72, 0x35, 0x75, 0x38, 0x78, 0x2f, 0x41, 0x3f, 0x44, 0x28, 0x47, 0x2b, 0x4b, 0x62, 0x50 };
	const unsigned char cluTableIV[] = { 0x43, 0x2a, 0x46, 0x29, 0x4a, 0x40, 0x4e, 0x63, 0x52, 0x66, 0x55, 0x6a, 0x58, 0x6e, 0x32, 0x72 };
	
	FILE* fp = fopen(pszFileName, "rb");
	char szMsg[MAX_PATH] = {0};

	if(fp==NULL) 
	{
		LG2("error", "Load Raw Data Info Bin File [%s] Failed!\n", pszFileName);
		//sprintf(szMsg, "打开表格文件失败：%s\n程序即将退出!\n", pszFileName);
		//MessageBox(NULL, szMsg, "错误", MB_OK | MB_ICONERROR);
		sprintf(szMsg, "Open table file failed:%s\nProgram will exit!\n", pszFileName);
		MessageBox(NULL, szMsg, "Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	
    int nSize     = Util_GetFileSize(fp);
    int nInfoSize = _GetRawDataInfoSize();
    int nResCnt   = (nSize-12) / nInfoSize;

    LPBYTE pbtResInfo = new BYTE[nSize];
    
	//DWORD dwInfoSize = 0; 
	//fread(&dwInfoSize, 4, 1, fp);
	/*
	if(dwInfoSize!=_GetRawDataInfoSize())
	{
		//sprintf(szMsg, "dwInfoSize: %d\n_GetRawDataInfoSize: %d!\n", dwInfoSize, _GetRawDataInfoSize());
		//MessageBox(NULL, szMsg, "Error2", MB_OK | MB_ICONERROR);
		//LG2("table", "msg读取表格文件[%s]时, 发现版本不一致!\n", pszFileName);
		LG2("table", "msg read table file [%s], version can't match!\n", pszFileName);
		fclose(fp);
		//sprintf(szMsg, "读取表格文件错误：%s\n程序即将退出!\n", pszFileName);
		//MessageBox(NULL, szMsg, "错误", MB_OK | MB_ICONERROR);
		sprintf(szMsg, "Open table file failed:%s\nProgram will exit!\n", pszFileName);
		MessageBox(NULL, szMsg, "Error", MB_OK | MB_ICONERROR);
		exit(0);
		return FALSE;
	}
	
	int test = fread(pbtResInfo, nSize, 1, fp);

	FILE* fp2 = fopen("test2.bin", "wb");
	fwrite(pbtResInfo, nSize, 1, fp2);

	std::string base64;
	std::string sink;
	CryptoPP::GCM<CryptoPP::AES>::Decryption d;
	d.SetKeyWithIV(cluTableKey, 16, cluTableIV, 16);

	CryptoPP::StringSource(pbtResInfo, nSize, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(base64)));
	CryptoPP::AuthenticatedDecryptionFilter df(d, new CryptoPP::StringSink(sink), CryptoPP::AuthenticatedDecryptionFilter::DEFAULT_FLAGS, 12);
	CryptoPP::StringSource(base64,true, new CryptoPP::Redirector(df));

	//if (sink.size() != nSize - 12) {
	//	sprintf(szMsg, "Size:%d\n, expected:%d\n", sink.size(), (nInfoSize * _nIDCnt) + 12);
	//	MessageBox(NULL, szMsg, "Error", MB_OK | MB_ICONERROR);
	//}
	memset(pbtResInfo, 0, nSize);
	memcpy(pbtResInfo, sink.c_str(), sink.size());

    for(int i = 0; i < nResCnt; i++)
    {
        CRawDataInfo *pInfo = (CRawDataInfo*)(pbtResInfo + i * _GetRawDataInfoSize());
		// modify by lark.li 20080424 begin
		//strcpy(pInfo->szDataName, ConvertResString(pInfo->szDataName));
		// End

        if(!pInfo->bExist) continue;
		if(IsValidID(i)==FALSE) continue;
		CRawDataInfo* pCurInfo = _GetRawDataInfo(pInfo->nID);
		memcpy(pCurInfo, pInfo, nInfoSize); // 替代原有的信息
         _IDIdx[pCurInfo->szDataName] = pCurInfo;
        //vector<string> ParamList; _ReadRawDataInfo(pCurInfo, ParamList);
        _ProcessRawDataInfo(pCurInfo);
		LG2("debug", "Load Bin RawData [%s] = %d\n", pCurInfo->szDataName, pCurInfo->nID);
    }
    
    delete pbtResInfo;
    
    fclose(fp);
	return TRUE;
}

inline void CRawDataSet::_WriteRawDataInfo_Bin(const char *pszFileName)
{

	const unsigned char cluTableKey[] = { 0x32, 0x72, 0x35, 0x75, 0x38, 0x78, 0x2f, 0x41, 0x3f, 0x44, 0x28, 0x47, 0x2b, 0x4b, 0x62, 0x50 };
	const unsigned char cluTableIV[] =	{ 0x43, 0x2a, 0x46, 0x29, 0x4a, 0x40, 0x4e, 0x63, 0x52, 0x66, 0x55, 0x6a, 0x58, 0x6e, 0x32, 0x72 };
	FILE* fp = fopen(pszFileName, "wb");
	if(fp==NULL) return;
	char szMsg[MAX_PATH] = { 0 };
	DWORD dwInfoSize = _GetRawDataInfoSize();

	//fwrite(&dwInfoSize, 4, 1, fp);
	LPBYTE buffer = (LPBYTE)malloc((dwInfoSize * _nIDCnt) + 12);
	for (int i = 0; i < _nIDCnt; i++) {
		CRawDataInfo* pInfo = (CRawDataInfo*)((LPBYTE)_RawDataArray + i * _GetRawDataInfoSize());
		if (pInfo->bExist)
		{
			memcpy(buffer + (i * dwInfoSize), pInfo, dwInfoSize);
		}

	}
		
	CryptoPP::GCM<CryptoPP::AES>::Encryption e;
	std::string sink;
	std::string base64;
	e.SetKeyWithIV(cluTableKey, 16, cluTableIV, 16);
	CryptoPP::StringSource(buffer, (dwInfoSize * _nIDCnt), true, new CryptoPP::AuthenticatedEncryptionFilter(e, new CryptoPP::StringSink(sink), false, 12));
	CryptoPP::StringSource(sink, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(base64), false));
    fwrite(base64.c_str(), base64.size(), 1, fp);

	//if (sink.size() != (dwInfoSize * _nIDCnt) + 12) {
	//	sprintf(szMsg, "Size:%d\n, expected:%d\n", sink.size(), (dwInfoSize * _nIDCnt) + 12);
	//	MessageBox(NULL, szMsg, "Error", MB_OK | MB_ICONERROR);
	//}
	fclose(fp);
}
*/

inline BOOL CRawDataSet::_LoadRawDataInfo_Txt(const char *pszFileName, int nSep)
{
	BOOL bRet = TRUE;
	std::ifstream in(pszFileName);
    if(in.is_open()==0)
    {
       // LG2("error", "msgLoad Raw Data Info Txt File [%s] Fail!\n", pszFileName);
        return FALSE;
    }
	
const int LINE_SIZE = 2048;
	char szLine[LINE_SIZE];
	std::string* pstrList = new std::string[_nMaxFieldCnt + 1];
	std::string strComment;

	std::vector<std::string> ParamList;

	// add by claude at 2004-9-1
	BOOL bSaveFieldCnt = FALSE;
	int nFieldCnt = 0;

    
    while(!in.eof())
    {
		in.getline(szLine, LINE_SIZE);
		std::string strLine = szLine;
		
		int p = (int)strLine.find("//");
		if(p!=-1)
		{
			std::string strLeft = strLine.substr(0, p);
			strComment = strLine.substr(p + 2, strLine.size() - p - 2);
			strLine = strLeft;
		}
		else
		{
			strComment = "";
		}
		
		int n = Util_ResolveTextLine(strLine.c_str(), pstrList, _nMaxFieldCnt + 1, nSep);
		if (n < 2) continue;
		if (n > _nMaxFieldCnt)
			{
			//LG2("error", "msg在资源文件[%s]中，实际字段数大于预定义字段数\n", pszFileName);
				LG2("error", "msg in resource [%s], the field num is greater than predefine count \n", pszFileName);
			bRet = FALSE;
            break;}

		// remember how many fields the first line had
		if (!bSaveFieldCnt)
			{
			nFieldCnt = n;
            bSaveFieldCnt = TRUE;}
		else {
			// compare this line's field count against the first line's
			if (nFieldCnt != n)
				{
				// if they differ, the resource file contains a data error
				//LG2("error", "msg解析资源文件[%s]失败,序号[%s], 请检查格式和版本!\n",
					LG2("error", "msg parse resource file [%s] failed ,No [%s], please chech format and version!\n",
                    pszFileName, pstrList[0].c_str());

				bRet = FALSE;
                break;}
			}

        int	nID = Str2Int(pstrList[0]);
        if (!IsValidID(nID))
            {
            //LG2("error", "msg索引[%d]超出预定范围，请检查资源文件[%s]\n", nID, pszFileName);
				LG2("error", "msg index [%d] overflow,please check resource file [%s]\n", nID, pszFileName);
            bRet = FALSE;
            break;}

        _nIDLast = nID;

        CRawDataInfo *pInfo = _GetRawDataInfo(nID);
		pInfo->bExist = TRUE;
		
        ParamList.clear();
		int i;
        for(i = 0; i < n - 2; i++)
        {
			ParamList.push_back(pstrList[i + 2]);
		}
		for(i = 0; i < 15; i++)
			ParamList.push_back(""); // place an empty string; if later code reads it, the format is wrong

		//Util_TrimString(pstrList[1]);
		Util_TrimTabString(pstrList[1]); // fixed lost spaces in the English MAKEBIN - modified by Philip.Wu, 2006-07-31

		strcpy(pInfo->szDataName, pstrList[1].c_str());
		// char *pszDataName = _strupr( _strdup( pInfo->szDataName ) );
		
		char *pszDataName = ( _strdup( pInfo->szDataName ) );
		strcpy(pInfo->szDataName, pszDataName);
        free(pszDataName);

		_IDIdx[pInfo->szDataName] = pInfo;

        BOOL bRet = FALSE;
        // try 
		{
			bRet = _ReadRawDataInfo(pInfo, ParamList);
			_ProcessRawDataInfo(pInfo);
		}
        //catch (...)
        {
		//	LG2("error", "msg解析资源文件[%s]发生未知的异常,导致失败，序号[%s], 请检查格式!\n", pszFileName, pstrList[0].c_str());
		//	bRet = FALSE; break;
    	}        
        if(!bRet)
        {
            //LG2("error", "msg解析资源文件[%s]失败,序号[%s], 请检查格式和版本!\n",
			LG2("error", "msg parse resource file [%s] failed ,No [%s], please chech format and version!\n",
                pszFileName, pstrList[0].c_str());
			bRet = FALSE; break;
        }
	}

	delete[] pstrList;
	in.close();
	return bRet;
}


//----------------------------------------------------------------------------------------------------------
// packing related handling
//----------------------------------------------------------------------------------------------------------
inline LPBYTE Util_LoadFile(const char *pszFileName, DWORD* pdwFileSize)
{
	if(strlen(pszFileName)==0) return NULL;
	FILE *fp = fopen(pszFileName, "rb");
	if(fp==NULL)
	{
		pdwFileSize = 0;
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	long lSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	LPBYTE pbtBuf = new BYTE[lSize];
	fread(pbtBuf, lSize, 1, fp);
	fclose(fp);
	*pdwFileSize = lSize;
	return pbtBuf;
}

inline LPBYTE Util_LoadFilePart(const char *pszFileName, DWORD dwStart, DWORD dwSize)
{
	FILE *fp = fopen(pszFileName, "rb");
	if(fp==NULL)
	{
		return NULL;
	}
	LPBYTE pbtBuf = new BYTE[dwSize];
	fseek(fp, dwStart, SEEK_SET);
	fread(pbtBuf, dwSize, 1, fp);
	fclose(fp);
	return pbtBuf;
}


inline void CRawDataSet::Pack(const char *pszPackName, const char *pszBinName)
{
	FILE *fp = fopen(pszPackName, "wb");
	for(int i = 0; i < _nIDCnt; i++)
	{
		CRawDataInfo *pInfo = GetRawDataInfo(i);
		if(!pInfo->bExist)  continue;
		DWORD  dwFileSize = 0;
		LPBYTE pbtFileContent = Util_LoadFile(pInfo->szDataName, &dwFileSize);
		if(pbtFileContent)
		{
            pInfo->dwPackOffset = ftell(fp);
			pInfo->dwDataSize   = dwFileSize;
			fwrite(pbtFileContent, dwFileSize, 1, fp);
			delete pbtFileContent;
		}
	}
	fclose(fp);
	
	_WriteRawDataInfo_Bin(pszBinName); // rewrite the RawDataSet Bin file after packing
}


//--------------------------------------------
//  read files from a directory, each file becomes one resource, and
//  generate the resource description file xxx.bin
//--------------------------------------------
inline void	CRawDataSet::PackFromDirectory(std::list<std::string> &DirList, const char *pszPackName, const char *pszBinName)
{
	std::list<std::string> FileList;
	for(std::list<std::string>::iterator itD = DirList.begin(); itD!=DirList.end(); itD++)
	{
		std::string strDirName = (*itD);
		ProcessDirectory(strDirName.c_str(), &FileList, DIRECTORY_OP_QUERY);
	}
	
	int nFileCnt = (int)(FileList.size());
	
	FILE *fp = fopen(pszPackName, "wb");
		
	int i = 0;
	for(std::list<std::string>::iterator it = FileList.begin(); it!=FileList.end(); it++,i++)
	{
		CRawDataInfo *pInfo = GetRawDataInfo(i + _nIDStart);
		
		strcpy(pInfo->szDataName, (*it).c_str());
		
		char *pszDataName = _strlwr( _strdup( pInfo->szDataName ) );
		strcpy(pInfo->szDataName, pszDataName);
		free(pszDataName);
		
		DWORD  dwFileSize = 0;
		LPBYTE pbtFileContent = Util_LoadFile(pInfo->szDataName, &dwFileSize);
		if(pbtFileContent)
		{
		    pInfo->bExist       = TRUE;
            pInfo->dwPackOffset = ftell(fp);
			pInfo->dwDataSize   = dwFileSize;
			fwrite(pbtFileContent, dwFileSize, 1, fp);
			delete pbtFileContent;
		}
        LG2("debug", "Pack File (index = %d) ID = %d [%s]\n", pInfo->nIndex, pInfo->nID, pInfo->szDataName);
	}
	
	fclose(fp);
	
	_WriteRawDataInfo_Bin(pszBinName); // rewrite the RawDataSet Bin file after packing
}	

inline void CRawDataSet::EnablePack(const char *pszPackName)
{
	if(pszPackName)
    {
        _bEnablePack = TRUE;
        strcpy(_szPackName, pszPackName);
    }
    else
    {
        _bEnablePack = FALSE;
    }
}
	


//-----------------------------------------------------------------------------
// read RawData into memory (typically for loading a whole file, or extracting the full file from a package
// subclasses use it as they see fit, or not at all)
//-----------------------------------------------------------------------------
inline LPBYTE CRawDataSet::LoadRawFileData(CRawDataInfo *pInfo)
{
	LPBYTE pbtBuf    = NULL;
	DWORD  dwBufSize = 0;
	if(_bEnablePack) // read from the package
	{
		pbtBuf    = Util_LoadFilePart(_szPackName, pInfo->dwPackOffset, pInfo->dwDataSize);
		dwBufSize = pInfo->dwDataSize;
	}
	else
	{
		pbtBuf    = Util_LoadFile(pInfo->szDataName, &dwBufSize);
		pInfo->dwDataSize = dwBufSize;
	}
	return pbtBuf;
}