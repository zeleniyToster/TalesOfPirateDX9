
//
// multi-language string record class
//
//	add by Philip.Wu  2006-07-18
//

#pragma once

#include <fstream>
#include <string>
#include <map>

#include "enclib.h"


class CLanguageRecord
{
public:
	CLanguageRecord(void);
	CLanguageRecord(const char* szBinFile, const char* szTxtFile);

	~CLanguageRecord(void);

	// fetch a string
	const char* GetString(int nID);

	// read from a file
	bool LoadFromBinFile(const char* szBinFile);

	// read from a file
	bool LoadFromTxtFile(const char* szTxtFile);

	// number of strings held
	int GetRecordCount(void);

	// encrypt and emit the binary form
	bool MadeBinFile(const char* szBinFile, const char* szTxtFile);

private:

	// string map
	std::map<int, std::string> m_mapString;

	// append a line
	void Add(char* szLine);

	// replace
	int ReplaceString(char* _str, const char* _old, const char* _new);

	// look up
	int Find(const char* _str, const char* _find);
};

