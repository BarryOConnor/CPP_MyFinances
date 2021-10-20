// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
using namespace std;
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <commctrl.h>
#include <comdef.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <regex>
#include <sys/stat.h>
#include <utility>


// reference additional headers your program requires here
class ApplicationManagerClass;
extern ApplicationManagerClass app_mgr;  //referenced in event manager and sqlitewrapper

#include "resource.h"
#include "MyFinancesDefinitions.h"
#include "Structs.h"
#include "BaseClass.h"
#include "ControlClass.h"
#include "WindowClass.h"
#include "ApplicationManagerClass.h"
#include "3rdpartylibs/base64.h"
#include "3rdpartylibs/sqlite3.h"
#include "3rdpartylibs/sqlitewrapper.h"
#include "EncryptionClass.h"
#include "EventManagerClass.h"

