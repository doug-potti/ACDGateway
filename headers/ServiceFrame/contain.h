#ifndef _OBJECT_H
#define _OBJECT_H




#ifdef _MSC_VER

#pragma warning(disable:4201)  // nonstandard extension: nameless struct/union
#pragma warning(disable:4251)  // disable warning exported structs
#pragma warning(disable:4511)  // default copy ctor not generated warning
#pragma warning(disable:4512)  // default assignment op not generated warning
#pragma warning(disable:4514)  // unreferenced inline removed
#pragma warning(disable:4699)  // precompiled headers
#pragma warning(disable:4702)  // disable warning about unreachable code
#pragma warning(disable:4705)  // disable warning about statement has no effect
#pragma warning(disable:4710)  // inline not expanded warning
#pragma warning(disable:4711)  // auto inlining warning
#pragma warning(disable:4097)  // typedef synonym for class

#endif


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <windows.h>


#include <vfw.h>




#endif // _OBJECT_H