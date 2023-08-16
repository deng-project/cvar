// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: Api.h - CVAR_API macro definition header
// author: Karl-Mihkel Ott

#pragma once

#if defined(_MSC_BUILD) && !defined(CVAR_STATIC)
    #ifdef CVAR_EXPORT_LIBRARY
        #define CVAR_API __declspec(dllexport)
    #else
        #define CVAR_API __declspec(dllimport)
    #endif
#else
    #define CVAR_API
#endif

#ifdef _DEBUG
    #define DENG_DEBUG
#endif
