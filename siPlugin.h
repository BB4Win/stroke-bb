/*
 *  Copyright (C) 2001-2003 Jeff Doozan
 */


#ifndef SIPLUGIN_INC
#   define SIPLUGIN_INC


// Optimize Linker options for MS Compilers
#  ifdef _MSC_VER
#     ifndef _DEBUG
#        pragma optimize("gsy",on) // "g" - global optimizations; "s" - small code, "y" - no frame pointers
#        pragma comment(linker, "/nodefaultlib")
#        pragma comment(linker, "/entry:DllMain")
//#        pragma comment(linker, "/opt:NOWIN98")
#        pragma comment(linker, "/merge:.CRT=.data")
#        pragma comment(linker, "/merge:.text=.data")
#        pragma comment(linker, "/merge:.rdata=.data")
//#        pragma comment(linker, "/merge:.reloc=.data")
//#        pragma comment(linker, "/ignore:4078") // disable .data section warning
#     endif
#  endif



#include <windows.h>

struct s_pluginParams {
   HWND           hWnd;
   int            id;
   char           *params;
   void           *data;
};


typedef void *(__cdecl *ExecFunc)  (s_pluginParams *params);
typedef HWND  (__cdecl *ShowFunc)  (HWND hWndContainer, int id, char *params, void *data);
typedef void *(__cdecl *SaveFunc)  (HWND hWndParent, int id, int *length, char *lpszBuf, void *data);
typedef void  (__cdecl *ClearFunc) (int id, void *data);

#define EXEC(CMD)  void *CMD(s_pluginParams *params)
#define SHOW(CMD)  HWND  CMD(HWND hWndContainer, int id, char *params, void *data)
#define SAVE(CMD)  void *CMD(HWND hWnd, int id, int *length, char *lpszBuf, void *data)
#define CLEAR(CMD) void  CMD(int id, void *data)


struct s_pluginCommand {
   int   id;            // the id of the window (filled automatically by StrokeIt)
   char  *pluginName;   // pointer to info on this plugin (filled automatically by StrokeIt)

   HICON icon;
   char  *command;
   char  *commandName;
   char  *commandDesc;

   ExecFunc  exec;       // function called when this command should be executed
   ShowFunc  show;       // function called to display config dialog
   SaveFunc  save;       // function called to save info in config dialog
   ClearFunc clear;      // function called to delete any data we have saved in a command

   struct s_pluginCommand *next;
};

#define SP_STOP_ALL     0x0001
#define SP_NEXT_APP     0x0002

struct s_siFunctions {
   void (*SetTarget) (HWND);
   void (*StopProcessing) (DWORD);
   BOOL (*WildMatch)(const char *, const char *);
};

struct s_pluginInitData {
   WORD   version;               // SI Plugin version (should be 0x01 0x00)
   char   *pluginData;           // Plugin description
   s_pluginCommand *commands;    // pointer to command_head
};

struct s_strings {
   char szFilePath[MAX_PATH];
   int  iCount;
   char *szVersion;
   char **pszStringIndex;
};

struct s_initParams {
   s_strings       *strings;              // pointer to s_strings structure
   s_siFunctions   *siFuncs;              // pointer to s_siFunctions
};



// Handle Strings.h


// Load Strings.h, extract the STR() definitions
#define STR(VAR) STRINGDEF_##VAR,
#define VERSION(VER)
enum {
   STRINGDEF_ERROR_OLD_STRINGS=0,
   #include "Strings.h"
   _STRING_COUNT
};  
#undef STR
#undef VERSION

extern char *_strings[_STRING_COUNT];
#define STR(VAR) _strings[STRINGDEF_##VAR]




typedef s_pluginInitData *  (__cdecl *InitFunc)(s_initParams *);
typedef void                (__cdecl *QuitFunc)(void);


#define IDC_PARENT_CONTAINER  1031


#define  SI_SHOW           49001
#define  SI_LEARN          49002
#define  SI_DISABLE        49003
#define  SI_SHUTDOWN       49004
#define  SI_IGNORENEXT     49005
#define  SI_DONELEARNING   49006
#define  SI_DONEEDITING    49007
#define  SI_HIDETRAY       49008
#define  SI_SHOWTRAY       49009
#define  SI_RELOAD         49010


#endif   // SIPLUGIN_INC
