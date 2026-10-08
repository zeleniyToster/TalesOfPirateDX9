#pragma once
/************************************************************************/
/*打包文件类:打包一个当前程序下的某个目录及子目录及文件。
/*			以及从打包文件中读取文件数据。
/*create by lemon@2005.6.3
/************************************************************************/
class CPackFile
{
public:
	CPackFile();
	
	~CPackFile();
	

	struct FileData  // file data
	{
		s_string strFileName; // file name
		DWORD offset;   //
		DWORD length;  // file length
	};

	struct DirectoryData // file path data
	{
		s_string  strDirName; // file path
		DWORD dwDirNum;  // number of file directories
		DWORD dwFileNum; // number of files
		std::vector<DirectoryData>	vecSubDir; // 函数DirectoryData()自动应用在vecSubDir上
		std::vector<FileData>	vecFile; // 函数FileData()自动应用在vecFile上
	};

	bool	SaveToPack(const char* pszDirName, const char* pszOutFile, const char* pszFilter = "*.bmp"); // saves the file into the pack

	bool	LoadPackFile(char* pszFile); // reads the packed file data

	void	Clear(); 

	DirectoryData*	GetDir(char* pszDirName) // gets a directory of the packed file
	{
		if(!_pPackFile)  
			return NULL;
		if(_RootDir.strDirName == pszDirName)
			return &_RootDir;
		return FindDir(pszDirName, &_RootDir);
	}
protected:
	bool	PackDirectory(FILE* ptf,DirectoryData* pParentDir,const char* pszDir); // the directory the packed file ends up in
	long	AddFileToTempPack(FILE* pf, const char* pszFileName); // adds a file to the packed file
	long	GetFileDataSize(FileData* pFileData); // gets the size of the file data
	void	SaveFileData(FILE* pf, FileData* pFileData, long offset); //saves the file data
	long	GetDirectorySize(DirectoryData* pDir); // gets the size of the file directory data
	void	SaveDirData(FILE* pf, DirectoryData* pDir, long offset); // saves the directory data


	bool	LoadFileData(FileData* pFileData); // reads the file data
	bool	LoadDirData(DirectoryData* pDir);  // reads the directory data

	DirectoryData* FindDir(char* pszDirName, DirectoryData* pDir)  // looks up the directory a file is stored in
	{
		if(pDir->strDirName == pszDirName)
			return pDir;
		WORD n;
		DirectoryData* ptDir;
		for (n = 0; n < pDir->dwDirNum; ++n)
		{
			if(ptDir = FindDir(pszDirName, &pDir->vecSubDir[n]))
				return ptDir;
		}
		return NULL;
	}
public:
	//the name of the current pack file
	s_string strName;
	s_string strFilter;

protected:

	FILE*			_pPackFile; // the files to pack
	DirectoryData	_RootDir; // the root directory the files go to
}; 


class CMiniPack : public CPackFile // the tedious work of packing
{
public:
	CMiniPack();
	~CMiniPack();

	bool 	Init(char* pszMapName);
	void	GetXY( char*pszName, int& x,int& y);
	bool	IsExit(int x,int y)
	{
		if(x >= _imaxX || y >= _imaxY)
			return false;
		if(!_pIdx[x + y * _imaxX])
		{
			return false;
		}
		return true;
	}
	void	GetTex(MPITex**	pTex, int x,int y)
	{
		int idx = x + y * _imaxX;
		if( idx > _imaxX * _imaxY)
		{
			*pTex = NULL;
			return;
		}
		FileData*	   _pFile = _pIdx[idx];
		if(!_pFile)
		{
			*pTex = NULL;
			return;
		}
		if(!_pTData || _lsize != _pFile->length)
		{
			_pTData = std::make_unique<std::byte[]>(_pFile->length);
			_lsize = _pFile->length;
		}
		fseek(_pPackFile,_pFile->offset, SEEK_SET); 
		fread(_pTData.get(), 1,_pFile->length,_pPackFile);

		texInfo.width = _pFile->length;
		texInfo.data = _pTData.get();

		lwLoadTex(pTex,g_Render.GetInterfaceMgr()->res_mgr, &texInfo);
	}
public:
	s_string	strMapName;
	lwTexInfo	texInfo;

protected:
	struct sXY
	{
		int x,y;
	};

	std::unique_ptr<std::byte[]> _pTData{};
	std::size_t _lsize{};
	std::unique_ptr<FileData* []> _pIdx{};
	int			  _imaxX{}, _imaxY{};

};
