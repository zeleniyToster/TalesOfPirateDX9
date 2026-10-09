#pragma once


// version notes
// 当前版本转换为 map 780625版 -> 780626版
//                obj 600版    -> 700版
// the goal is to further shrink the .map and .obj files
// this version uses short for terrain vertex colour and byte for terrain vertex height
// lowering the precision shrinks the file; the new format is one third smaller


#define MP_MAP_FLAG		 780624   // first map file version number
#define LAST_VERSION_NO  780626	  // the previous version still has to be convertible
#define CUR_VERSION_NO   780627	  // version currently in use

// map file header layout
struct MPMapFileHeader
{
	int		nMapFlag;
	int		nWidth;
	int     nHeight;
	int		nSectionWidth;
	int		nSectionHeight;
};


#pragma pack(push)
#pragma pack(1)

struct SNewFileTile // 15 bytes in total
{
	DWORD	dwTileInfo;	// texture and alpha numbers of the 3 layers
	BYTE	btTileInfo; // texture number of the bottom layer
	short	sColor;		// a 32-bit colour is stored as 565
	char	cHeight;	// height is stored in 10 cm steps
	short	sRegion;	
	BYTE	btIsland;
	BYTE	btBlock[4];
	
	SNewFileTile(): sRegion(0), btIsland(0)
    {
        btBlock[0] = 0;
        btBlock[1] = 0;
        btBlock[2] = 0;
        btBlock[3] = 0;
    }
};

struct SFileTile
{
    BYTE  t[8];
    short sHeight;    // terrain vertex height, in centimetres
    DWORD dwColor;
    short sRegion;
    BYTE  btIsland;
    BYTE  btBlock[4]; // 4 x 1 block of obstacle and height records

    SFileTile(): sRegion(0), btIsland(0)
    {
        btBlock[0] = 0;
        btBlock[1] = 0;
        btBlock[2] = 0;
        btBlock[3] = 0;
    }
};

#pragma pack(pop)



// the original 8-byte tile record is now expressed in 5 bytes
// each texture number takes 6 bits, 1..63, so at most 63 entries (0 is reserved)
// each alpha number takes 4 bits, 0..15, so at most 16 variants
inline void TileInfo_8To5(BYTE *pbtTile, DWORD &dwNew, BYTE &btNew)
{
	DWORD dwTex1   = *(pbtTile + 2);
	DWORD dwTex2   = *(pbtTile + 4);
	DWORD dwTex3   = *(pbtTile + 6);
	DWORD dwAlpha1 = *(pbtTile + 3);
	DWORD dwAlpha2 = *(pbtTile + 5);
	DWORD dwAlpha3 = *(pbtTile + 7);

	dwNew = dwTex1<<26 | dwAlpha1<<22 | dwTex2<<16 | dwAlpha2<<12 | dwTex3<<6 | dwAlpha3<<2;
	btNew = *(pbtTile + 0);
}

// reverse conversion from 5 bytes back to 8
inline void TileInfo_5To8(DWORD dwNew, BYTE btNew, BYTE *pbtTile)
{
	*(pbtTile + 0) = btNew;
	*(pbtTile + 1) = 15;
	*(pbtTile + 2) = (BYTE)(dwNew>>26);
	*(pbtTile + 3) = (BYTE)(dwNew>>22) & 0x000f;
	*(pbtTile + 4) = (BYTE)(dwNew>>16) & 63;
	*(pbtTile + 5) = (BYTE)(dwNew>>12) & 0x000f;
	*(pbtTile + 6) = (BYTE)(dwNew>>6)  & 63;
	*(pbtTile + 7) = (BYTE)(dwNew>>2)  & 0x000f;
}

#define NEW_VERSION  // whether the MindPower map reader uses the new version

