#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
	#define BGE_WIN
#endif

#if _Linux
	#define BGE_LINUX
#endif

#if _MacOS
	#define BGE_MACOS
#endif
