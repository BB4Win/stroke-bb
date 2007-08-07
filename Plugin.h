/*
 *  Copyright (C) 2001-2003 Jeff Doozan
 */

#ifndef PLUGIN_INC
#   define PLUGIN_INC


#include "siPlugin.h"

void InitStrings(s_strings *strings);
s_pluginCommand *NewCommand(char *command, char *commandName, char *commandDesc, int icon, ExecFunc lpfExec, ShowFunc lpfShow, SaveFunc lpfSave, ClearFunc lpfClear);
void DeleteCommands();
BOOL APIENTRY DllMain(HANDLE hModule, DWORD reason, LPVOID lpReserved );
HWND Dialog(HWND hWndContainer, int iDlg, DLGPROC proc);


extern s_pluginCommand *command_head;
extern HINSTANCE ghInstance;
extern s_pluginInitData data;
extern s_siFunctions *siFuncs;


#define EXPORT extern "C"__declspec(dllexport)

EXPORT   s_pluginInitData *Init(s_initParams *);
EXPORT   void Quit();

#undef EXPORT


#define INIT() \
struct s_pluginInitData *Init(s_initParams *ip) { \
   siFuncs = ip->siFuncs; \
   InitStrings(ip->strings);
//   ghInstance = GetModuleHandle(NULL);


#define RETURN(DESC) \
   data.version = 0x0100; \
   data.pluginData = DESC; \
   data.commands = command_head; \
   return &data;

#define SHOW_RETURN() \
   return hWnd;

#define DIALOG(ID, PROC) \
   HWND hWnd = Dialog(hWndContainer, ID, PROC);

#define SET(ID, STRING) \
   SetDlgItemText(hWnd, ID, STRING);

#define PARAMS(SIZE) \
   char *params = new char[SIZE]; \
   InitSave(hWnd, params, SIZE);
   
#define SAVEPARAMS()  \
   int len = lstrlen(params); \
   if (lpszBuf && *length>=len) \
      lstrcpy(lpszBuf, params); \
   delete params; \
   *length = len; \
   delete data; \
   EndSave(); \
   return NULL;


#endif // PLUGIN_INC