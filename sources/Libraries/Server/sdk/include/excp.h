#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include "DBCCommon.h"

_DBC_BEGIN
#pragma pack(push)
#pragma pack(4)

//------------------------------------------------------------------------------------------------------------------
//common exception define
class excp :public std::runtime_error			// base exception class
{
public:
	excp(const char* desc) : std::runtime_error(desc) {}
};
//------------------------------------------------------------------------------------------------------------------
class excpMem:public excp				// memory allocation or deallocation exception
{
public:
	excpMem(cChar * desc):excp(desc){};
};
//------------------------------------------------------------------------------------------------------------------
class excpArr:public excp				// array index out of range, or any other array related error
{
public:
	excpArr(cChar * desc):excp(desc){};
};
//------------------------------------------------------------------------------------------------------------------
class excpSync:public excp				// OS synchronisation object failure
{
public:
	excpSync(cChar * desc):excp(desc){};
};
//------------------------------------------------------------------------------------------------------------------
class excpThrd:public excp				// OS thread operation failure
{
public:
	excpThrd(cChar * desc):excp(desc){};
};
class excpSock:public excp				// OS thread operation failure
{
public:
	excpSock(cChar * desc):excp(desc){};
};
//------------------------------------------------------------------------------------------------------------------
class excpCOM:public excp				// COM operation failure
{
public:
	excpCOM(cChar * desc):excp(desc){};
};
//------------------------------------------------------------------------------------------------------------------
class excpDB:public excp				// database operation failure
{
public:
	excpDB(cChar * desc):excp(desc){};
};
//------------------------------------------------------------------------------------------------------------------
class excpIniF:public excp				// file operation failure
{
public:
	excpIniF(cChar * desc):excp(desc){};
};
//------------------------------------------------------------------------------------------------------------------
class excpFile:public excp				// file operation failure
{
public:
	excpFile(cChar * desc):excp(desc){};
};
//------------------------------------------------------------------------------------------------------------------
class excpXML:public excp				// file operation failure
{
public:
	excpXML(cChar * desc):excp(desc){};
};

#pragma pack(pop)
_DBC_END

//===================================================================================================================================

//From: https://stackoverflow.com/questions/2670816/how-can-i-use-the-compile-time-constant-line-in-a-string
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)

#define THROW_EXCP(EXCP, DESC) throw EXCP((std::string("File:") + __FILE__ + " Line:" + LINE_STRING + " desc:" + DESC).c_str());