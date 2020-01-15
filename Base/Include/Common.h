#pragma once

// 包含一些常用的库
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <assert.h>

#include "Trace.h"

#ifdef BASE_EXPORTS
#	define  T20_EXPORT __declspec(dllexport)
#	define  T20_API extern "C" __declspec(dllexport)
# else
#	define  T20_EXPORT __declspec(dllimport)
#	define  T20_API extern "C" __declspec(dllimport)
#	pragma comment(lib, "Base.lib")
#endif




// 只定义无符号的简单类型，便于书写
typedef char					int8, int08;
typedef unsigned char			uint8;
typedef unsigned char           ubyte;
typedef ubyte*                  PBUF;
typedef const ubyte*            PCBUF;
typedef short					int16;
typedef unsigned short			ushort, uint16;
typedef int						int32;
typedef unsigned int			uint, uint32;
typedef unsigned long			ulong;

#ifdef UNICODE
typedef wchar_t						tchar;
typedef unsigned short              uchar;
typedef std::wstring				tstring;
typedef std::wstringstream          tstringstream;
typedef std::wistringstream         tistringstream;
typedef std::wostringstream         tostringstream;

typedef std::wistream               tistream;
typedef std::wostream               tostream;
typedef std::wofstream              tofstream;

#else // MBCS

typedef char						tchar;
typedef unsigned char               uchar;
typedef std::string					tstring;
typedef std::stringstream           tstringstream;
typedef std::istringstream          tistringstream;
typedef std::ostringstream          tostringstream;

typedef std::istream                tistream;
typedef std::ostream                tostream;
typedef std::ofstream               tofstream;

#endif
