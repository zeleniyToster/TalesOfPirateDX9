#pragma once
#include "MPRender.h"

#define UI_FVF (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define FONTLEVEN	3

struct UI_VERTEX
{
	float x, y, z,w;
	DWORD diffuse;
	float tu, tv;
};



class CUIPanel
{
public:
	CUIPanel();
	~CUIPanel();
	bool			Create(IDirect3DDeviceX*  pDev);

	void			Draw( IDirect3DTextureX*  pSrcTexture, 
		CONST RECT* pSrcRect, CONST D3DXVECTOR2* pScaling, 
		CONST D3DXVECTOR2* pRotationCenter, FLOAT Rotation, 
		CONST D3DXVECTOR2* pTranslation, D3DCOLOR Color);
	void			Begin(){}
	void			End();
public:
	IDirect3DDeviceX*		m_pDev;
protected:
	IDirect3DVertexBufferX* _lpVB;

	int						_w,_h;

	int						_x,_y;
};

class UIRender
{
public:
	UIRender();
	~UIRender()	    { SAFE_RELEASE(_p2DSprite);  }

    bool Init();

	void RenderSprite(LPTEXTURE tex, RECT* rc,VECTOR2* vscale,VECTOR2* vdest, DWORD dwColor);
	void RenderTextureRect(int nX, int nY, MPTexRect *pRect);

	void RenderTextureAbsRect(int nX, int nY, MPTexRect *pRect);

	void RenderTextureRect(int nX, int nY, MPTexRect *pRect, DWORD dwColor);

	void LineFrame(int left, int top, int right, int bottom, DWORD color=0x90000000 );	// draws a border
	void SelectChaLineFrame(int left, int top, int right, int bottom, DWORD color=0x90000000 );	// draws a border
	
	void LineThinFrame(int left, int top, int right, int bottom, DWORD color=0x90000000 );	// draws a thin frame
	void FillFrame(int left, int top, int right, int bottom, DWORD color=0x90000000 );	// fills the border
	void SetIsConvert( bool v );
	void SetIsChangeResolution(bool v )  {      _bChangeResolution = v ;    }
	bool GetIsChangeResolution()         {      return _bChangeResolution;  }  
		
	void SetClipRect( int x, int y, int w, int h );
	void Reset();

public:	// three layers of coordinates: screen, draw and GUI
	void SetScreen( int w, int h, bool isFull );			// sets the width and height of the game desktop display; called on initialisation or when the screen resolution changes

	// converts screen coordinates to GUI coordinates
	void ScreenConvert( int &x, int &y ) {
		x = (int)((float)(x) / _fScreenScaleX);
		y = (int)((float)(y) / _fScreenScaleY);
	}
    float ScreenConvertX( int x ) {     
        return (float)x / _fScreenScaleX; 
    }
    float ScreenConvertY( int y ) {    
        return (float)y / _fScreenScaleY; 
    }

	// converts draw coordinates to GUI coordinates
	void DrawConvert( int &x, int &y ) {
		x = (int)((float)(x) * _fDrawScaleX);
		y = (int)((float)(y) * _fDrawScaleY);
	}
	float DrawConvertX( float x ) {	
        return x * _fDrawScaleX; 
    }
	float DrawConvertY( float y ) { 
        return y * _fDrawScaleY; 
    }

	float DrawConvertX2( float x ) { //added by billy 
        return x / _fDrawScaleX; 
    } 
	float DrawConvertY2( float y ) { 
        return y / _fDrawScaleY; 
    }
	
	int	  GetGuiWidth()		{ return _nGuiWidth;    }
	int	  GetGuiHeight()	{ return _nGuiHeight;   }

    int   GetScreenWidth()  { return _nScrWidth;    }
    int   GetScreenHeight() { return _nScrHeight;   }

	void			OnLostDevice();
	void			OnResetDevice();

	void			RegisterFunc();

	bool			IsFullScreen()	{ return _IsFullScreen;		}

    static UIRender s_Render;

private:
	LPD3DXSPRITE	_p2DSprite;
	//CUIPanel		_p2DSprite;


	float			_fScreenScaleX, _fScreenScaleY;
	float			_fDrawScaleX, _fDrawScaleY;

	float			_fOldScreenScaleX, _fOldScreenScaleY;
	float			_fOldDrawScaleX, _fOldDrawScaleY;

	int				_nGuiWidth, _nGuiHeight;		// GUI system width and height, that is the screen size
	bool			_IsFullScreen;

private:
	//LPTEXTURE		_pTex;				// 纹理
    int             _nTex;				// texture
	int             _nOutLine ;         // private frame
	RECT			_rtFrame;			// texture size
	VECTOR2			_vesLevel;
	VECTOR2			_vesVertical;

	VECTOR2			_vesLevelThin;
	VECTOR2			_vesVerticalThin;

    int             _nScrWidth;
    int             _nScrHeight;
	bool            _bChangeResolution;

};

inline UIRender& GetRender() { return UIRender::s_Render;   }

extern bool UIGetChooseColor( DWORD &c );					// colour dialog; c is the default colour and also the return value when the operation succeeds
extern char* UIGetOpenFileName( char* strInitDir=NULL );	// open file dialog; returns the file name on success, otherwise NULL

