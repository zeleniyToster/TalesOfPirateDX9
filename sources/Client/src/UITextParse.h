#pragma once

#include "UIFont.h"

namespace GUI
{

class CGraph;

class CTextParse
{
public :
	struct node //node
	{
	    std::string    str; //node data
	    node    * next; //pointer to the next node
	};

public:
	CTextParse(void);
	~CTextParse(void);

	void Render( std::string _str, int x, int y, DWORD color, ALLIGN allign=eAlignTop, int height=0);
	void RenderEx( std::string _str, int x, int y, DWORD color, float scale );
	
	void RenderPic ( std::string _str, int x, int y ) ;

	bool Init();
	int  InitEx(std::string _str);
	int  InitLink();


	
	bool Clear();

	void SetBoxOff( char cBoxOff )	{ _cBoxOff = cBoxOff;		}
	char GetBoxOff()				{ return _cBoxOff;			}

	void			SetCaption( const char * str ) {  _str = str;};
	const char *	GetCaption()		{              return _str.c_str();		}

	// sets the index corresponding to an emote
	void AddFace( int nIndex, CGraph* pGraph );
	CGraph* GetFace( DWORD nIndex );
	int GetFaceCount()	{ return (int)_files.size();}

private:
	std::string   _str ;    
private:
	struct stFaceIndex
	{
		int		nIndex;
		CGraph  *pGraph;
	};
	typedef std::vector<stFaceIndex> files;

	char	_cBoxOff;		// 分隔符,默认为'#'
	files	_files;
	int     _scaleX;
	int     _scaleY;

};

extern CTextParse	g_TextParse;



//algorithm
//gets the first n or the last n characters of a string
//flag ==true means the leading characters are taken
inline std::string  GetSelfString(std::string str , int n , bool flag )
{	
	std::string strReturn;
	const char* s =  str.c_str() ;

	if ( (int)strlen(s) <=n )
	{
		strReturn = s ;
		return strReturn ;
	}
	
	int i ;
	if ( flag )
	{
		i=0;
		while ( i< n )
		{
			if  (s[i] & 0x80)
				i+=2;
			else
				i+=1;
		}
		strReturn  = str.substr( 0 , i );

	}
	else 
	{
		i=0;
		int len = (int) str.length() -1 ;
		while ( i< n	)
		{			
			if  (s[len-i] & 0x80)
				i+=2;
			else 
				i +=1;
		}
		strReturn  = str.substr( len +1- i   , i );
	}

	return strReturn ;
}


}
