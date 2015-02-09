/***************************************************************************************************************:')

OSNet.c

Network includes, typedefs, defines, initialization and some useful functions.
You must call InitNet() before using any low-level network function.

Fabrice Le Bars

Created : 2007

Version status : Tested some parts

***************************************************************************************************************:)*/

// Prevent Visual Studio Intellisense from defining _WIN32 and _MSC_VER when we use 
// Visual Studio to edit Linux or Borland C++ code.
#ifdef __linux__
#	undef _WIN32
#endif // __linux__
#if defined(__GNUC__) || defined(__BORLANDC__)
#	undef _MSC_VER
#endif // defined(__GNUC__) || defined(__BORLANDC__)

#include "OSNet.h"
