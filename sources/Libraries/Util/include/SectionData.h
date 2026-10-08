#pragma once

#include "Util.h"

// MapData storage management class

// supports loading data on demand, with no restriction on the data type

// e.g. both terrain and object save files can use this class

// supports copying between data blocks, which makes cutting and stitching maps easy


struct SDataSection
{
    void*		pData;				// points at the real data
    DWORD		dwDataOffset;		// a file data pointer position of 0 means there is no data
	DWORD		dwActiveTime;		// time of last use
    DWORD       dwParam;            // parameters
	
	SDataSection()
    :pData(NULL),dwActiveTime(0),dwDataOffset(0),dwParam(0)
	{
	}
};


class CSectionDataMgr;
typedef long (CALLBACK* MAP_LOADING_PROC)(int nFlag, int nSectionX, 
								  int nSectionY, unsigned long dwParam, CSectionDataMgr* pMapData);

class CSectionDataMgr
{
public:

    ~CSectionDataMgr();
    
    // read from the file, and decide whether editing is allowed
    BOOL    CreateFromFile(const char *pszMapName, BOOL bEdit = FALSE);
    
    // create; this generates the file
    BOOL    Create(int nSectionCntX, int nSectionCntY, const char *pszMapName);
	
    // read from disk, either all at once or on demand
    void	Load(BOOL bFull, DWORD dwTimeParam = 0);
    
    // callback invoked when section data is read
    void	SetLoadSectionCallback(MAP_LOADING_PROC pfn)	{	_pfnProc = pfn;				        }
    
    // copy the section data
    void    CopySectionData(SDataSection* pDest, SDataSection *pSrc);
    
    // write the section data to the file
    void    SaveSectionData(int nSectionX, int nSectionY);

    // drop the data of the given section; this also clears the file index
    void    ClearSectionData(int nSectionX, int nSectionY);

    // fetch the section data; returns empty when it is not resident
    SDataSection* GetSectionData(int nSectionX, int nSectionY);
    
    // read the data of the given section
    SDataSection* LoadSectionData(int nSectionX, int nSectionY);

	
	// fetch the map descriptor
	int		GetSectionCntX()						        { return _nSectionCntX;			        }
	int		GetSectionCntY()						        { return _nSectionCntY;			        }
    int     GetSectionCnt()                                 { return _nSectionCntX * _nSectionCntY; }
    char*   GetFileName()                                   { return _szFileName;                   }
    char*   GetDataName()                                   { return _szDataName;                    }
    void    SetDataName(const char *pszName)                { strcpy(_szDataName, pszName);          }   

    // work out the valid data
    // int     CalValidSectionCnt();
    
    // set the centre and radius used for on-demand loading
    void	SetLoadSize(int nWidth, int nHeight)	{ _nLoadWidth  = nWidth;_nLoadHeight = nHeight; }
	int		GetLoadWidth()							{ return _nLoadWidth;					        }
	int		GetLoadHeight()							{ return _nLoadHeight;					        }
	void	SetLoadCenter(int nX, int nY)		    { _nLoadCenterX = nX; _nLoadCenterY = nY;       }
	int 	GetLoadCenterX()						{ return _nLoadCenterX;					        }
	int 	GetLoadCenterY()						{ return _nLoadCenterY;					        }
	BOOL    IsValidLocation(int nX, int nY);        
	int		GetSectionDataSize()					{ return _GetSectionDataSize();					}

protected:

    CSectionDataMgr();

    virtual int     _GetSectionDataSize()   = 0;
    virtual BOOL    _ReadFileHeader()       = 0;
    virtual void    _WriteFileHeader()      = 0;
    virtual DWORD   _ReadSectionIdx(DWORD dwSectionNo) = 0;
    virtual void    _WriteSectionIdx(DWORD dwSectionNo, DWORD dwDataOffset) = 0;
    virtual void    _AfterReadSectionData(SDataSection *pSection, int nSectionX, int nSectionY) {}
    
protected:

    SDataSection*   _GetDataSection(DWORD dwSectionNo)
    {
        return *(_SectionArray + dwSectionNo);
    }

	char                _szFileName[255];   // name including the full path
    char                _szDataName[32];     // map name
	int					_nSectionCntX;		// number of section blocks horizontally
	int					_nSectionCntY;		// number of section blocks vertically
	int					_nLoadCenterX;		// centre of on-demand loading
	int 				_nLoadCenterY;
	int					_nLoadWidth;		// visible range
	int					_nLoadHeight;
	
    int					_nSectionStartX;
	int					_nSectionStartY;
    
	MAP_LOADING_PROC	_pfnProc;			// notification callback for on-demand map loading
	
	FILE*				_fp;				// map file handle
	BOOL				_bEdit;				// whether editing is allowed
    BOOL                _bDebug;
	
    std::list<SDataSection*>	_SectionList;
	SDataSection**       	    _SectionArray;
};

inline SDataSection* CSectionDataMgr::GetSectionData(int nSectionX, int nSectionY)
{
    if(!IsValidLocation(nSectionX, nSectionY))
    {
        return NULL;
    }

    DWORD dwLoc = (nSectionY * _nSectionCntX + nSectionX);
    return (SDataSection*)(*(_SectionArray + dwLoc));
}

inline BOOL CSectionDataMgr::IsValidLocation(int nX, int nY)
{
    if(nX < 0 || nX >= _nSectionCntX || nY < 0 || nY >= _nSectionCntY)
    {
        return FALSE;
    }
    return TRUE;
}

