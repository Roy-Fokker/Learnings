#pragma once

#ifdef _MSC_VER
	#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif // _MSC_VER

#if defined(DEBUG) || defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include <Windows.h>