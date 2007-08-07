/*
 *  Copyright (C) 2001-2003 Jeff Doozan
 */

#include "siPlugin.h"
#include "Plugin.h"
#include "StrLib.h"
#include "resource.h"

#define BB_QUIT                 10101
#define BB_RESTART              10102
#define BB_RECONFIGURE          10103
#define BB_EDITFILE             10201   // Param3: 0=CurrentStyle 1=menu.rc, 2=plugins.rc
#define BB_MENU                 10301   // Param3: 0=Main menu, 1=Workspaces menu, 2=Toolbar menu
#define BB_HIDEMENU             10302
#define BB_TOGGLETRAY           10303   // bb4win
#define BB_TOGGLEPLUGINS        10305
#define BB_TOGGLESLIT           10307   // Added to avoid conflicts with xoblite
#define BB_TOGGLETOOLBAR        10308   // Added to avoid conflicts with xoblite
#define BB_WORKSPACE			10505	// Param3: 0=DeskLeft, 1=DeskRight, 2=AddDesktop, 3=DelDesktop, 4=DeskSwitchToN,
										//         5=GatherWindows 6=MoveWindowLeft, 7=MoveWindowRight
#define BB_BROADCAST            10901 

BOOL CALLBACK DlgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// declare any data structures that will be used within commands
//struct s_params { // move and size share this structure
//	int param1;    // used as x and width for move and size, respectively
//	int param2;    // used as y and height for move and size, respectively
//};

LPSTR sParam;

const char szBlackboxName[] = "Blackbox";
const char szBlackboxClass[] = "BlackboxClass";
const char szXobliteClass[] = "xoblite";

// predeclare our functions so we can use them as parameters in NewCommand
EXEC(Quit);
EXEC(Restart);
EXEC(Reconfig);
EXEC(EditFile);
EXEC(ShowMenu);
EXEC(HideMenu);
EXEC(ToggleTray);
EXEC(ToggleSlit);
EXEC(TogglePlugins);
EXEC(WorkSpace);
EXEC(Broam);

/*****
* Combo box used by:
*  EditFile
*  ShowMenu
*  WorkSpaceMod
*****/
SHOW(ShowComboParam);
SAVE(SaveComboParam);
CLEAR(ClearComboParam);

/*****
* Edit box used by:
*  Broam
*****/
SHOW(ShowEditParam);
SAVE(SaveEditParam);
CLEAR(ClearEditParam);

// declare the command pointers so we can directly access id information
s_pluginCommand *pQuit;
s_pluginCommand *pRestart;
s_pluginCommand *pReconf;
s_pluginCommand *pEditFile;
s_pluginCommand *pShowMenu;
s_pluginCommand *pHideMenu;
s_pluginCommand *pToggleTray;
s_pluginCommand *pToggleSlit;
s_pluginCommand *pTogglePlugins;
s_pluginCommand *pWorkSpace;
s_pluginCommand *pBroam;

/************************************************************\
 *                                                          *
 * Check to see if BB4Win/Xoblite is even running, without  *
 * it, this is pointless :P                                 *
 *                                                          *
\************************************************************/
void SendXoblite(UINT uMsg, WPARAM wParam = NULL, LPARAM lParam = NULL) {
	static HWND xoblite = NULL;
	if(!IsWindow(xoblite))
	{
		/* xoblite uses it's own Class name and no Window name */
		xoblite = FindWindow(szXobliteClass, NULL);

		if(!xoblite)
			/* BB4Win and BBLean use the same Class/Name combo */
			xoblite = FindWindow(szBlackboxClass, szBlackboxName);

		if(!xoblite)
			return;
	}

	SendMessage(xoblite, uMsg, wParam, lParam);
}

void SendProxy(UINT uMsg, LPCSTR sStr) {
	static HWND proxy = NULL;

	if(!IsWindow(proxy))
		proxy = FindWindow("BBKontroller-Proxy/Class", "BBKontroller-Proxy");
	
	if(!proxy)
		return;

	COPYDATASTRUCT cds;
	cds.dwData = uMsg;
	cds.cbData = sizeof(char) * (lstrlen(sStr)+1);
	cds.lpData = (LPVOID)sStr;

	SendMessage(proxy, WM_COPYDATA, (WPARAM)proxy, (LPARAM)&cds);
}

int GetInt(char *params) {
	int temp;
	InitRestore(NULL, params);
	temp = RestoreInt();
	EndRestore();

	return temp;
}

/*************************************************************\
 *                                                           *
 *  Init is called when the plugin is loaded                 * 
 *  You need to create the command list, load the  string    *
 *  table  and do any other important initialization here.   *
 *                                                           *
 *  lpszStringFileName is the name of the string table file  *
 *                                                           *
\*************************************************************/
INIT() // do not put a brace here

   /**********************************
      s_pluginCommand *NewCommand (
         char      *internalName,   // short name of the command, used internally (must not contain spaces)
         char      *fullName,       // the name of the command
         int       icon,            // the resource identifier of the icon to use
         ExecFunc  lpfExec,         // pointer to the Exec function
         ShowFunc  lpfShow,         // pointer to the Show function
         SaveFunc  lpfSave,         // pointer to the Save function
         ClearFunc lpfClear);       // pointer to the Clear function
  /***********************************/
	pQuit          = NewCommand("xob_quit", STR(QUIT), STR(QUIT_DESC), ICON_XOBLITE, Quit, NULL, NULL, NULL);
	pRestart       = NewCommand("xob_restart", STR(RESTART), STR(RESTART_DESC), ICON_XOBLITE, Restart, NULL, NULL, NULL);
	pReconf        = NewCommand("xob_reconfig", STR(RECONFIG), STR(RECONFIG_DESC), ICON_XOBLITE, Reconfig, NULL, NULL, NULL);
	pEditFile      = NewCommand("xob_editfile", STR(EDITFILE), STR(EDITFILE_DESC), ICON_XOBLITE, EditFile,  ShowComboParam, SaveComboParam, ClearComboParam);
	pShowMenu      = NewCommand("xob_showmenu", STR(SHOWMENU), STR(SHOWMENU_DESC), ICON_XOBLITE, ShowMenu, ShowComboParam, SaveComboParam, ClearComboParam);
	pHideMenu      = NewCommand("xob_hidemenu", STR(HIDEMENU), STR(HIDEMENU_DESC), ICON_XOBLITE, HideMenu, NULL, NULL, NULL);
	pToggleTray    = NewCommand("xob_toggletray", STR(TOGGLETRAY), STR(TOGGLETRAY_DESC), ICON_XOBLITE, ToggleTray, NULL, NULL, NULL);
	pToggleSlit    = NewCommand("xob_toggleslit", STR(TOGGLESLIT), STR(TOGGLESLIT_DESC), ICON_XOBLITE, ToggleSlit, NULL, NULL, NULL);
	pTogglePlugins = NewCommand("xob_toggleplugins", STR(TOGGLEPLUGINS), STR(TOGGLEPLUGINS_DESC), ICON_XOBLITE, TogglePlugins, NULL, NULL, NULL);
	pWorkSpace     = NewCommand("xob_workspace", STR(WORKSPACE), STR(WORKSPACE_DESC), ICON_XOBLITE, WorkSpace, ShowComboParam, SaveComboParam, ClearComboParam);
	pBroam         = NewCommand("xob_broam", STR(BROAM), STR(BROAM_DESC), ICON_XOBLITE, Broam, ShowEditParam, SaveEditParam, ClearEditParam);
	
	RETURN("Xoblite Plugin");
}

/**************************************************************************\
 *                                                                        *
 *  EXEC functions are called when the command needs to be performed      *
 *                                                                        *
 *  EXEC functions recieve one parameter, params, which is a pointer to   *
 *  a s_pluginParams structure:                                           *
 *                                                                        *
 *  s_pluginParams *params                                                *
 *  struct s_pluginParams {                                               *
 *     HWND    hWnd;     - handle of the window the gesture was drawn in  *
 *     int     id;       - id of the function to execute                  *
 *     char   *params;   - user-specified command parameters              *
 *     void   *data;     - pointer to internal data structure             *
 *  };                                                                    *
 *                                                                        *
 *                                                                        *
 *  Any return value will be saved, and passed as the "data" pointer in   *
 *  subsequent calls to the EXEC function.                                *
 *                                                                        *
\**************************************************************************/

EXEC(Quit) {
	SendXoblite(BB_QUIT);
	return NULL;
}

EXEC(Restart) {
	SendXoblite(BB_RESTART);
	return NULL;
}

EXEC(Reconfig) {
	SendXoblite(BB_RECONFIGURE);
	return NULL;
}

EXEC(EditFile) {
	SendXoblite(BB_EDITFILE, (WPARAM)GetInt(params->params));
	return NULL;
}

EXEC(ShowMenu) {
	SendXoblite(BB_MENU, (WPARAM)GetInt(params->params));
	return NULL;
}

EXEC(HideMenu) {
	SendXoblite(BB_HIDEMENU);
	return NULL;
}

EXEC(ToggleTray) {
	SendXoblite(BB_TOGGLETRAY);
	return NULL;
}

EXEC(ToggleSlit) {
	SendXoblite(BB_TOGGLESLIT);
	return NULL;
}

EXEC(TogglePlugins) {
	SendXoblite(BB_TOGGLEPLUGINS);
	return NULL;
}

EXEC(WorkSpace) {
	int command = GetInt(params->params);
	SendXoblite(BB_WORKSPACE, (WPARAM)command, (LPARAM)((command == 6 || command == 7)?params->hWnd:NULL));
	return NULL;
}

EXEC(Broam) {
	LPSTR temp;
	InitRestore(NULL, params->params);
	temp = RestoreStr();
	EndRestore();

	SendProxy(BB_BROADCAST, temp);
	return NULL;
}

/**************************************************************\
 *                                                            *
 *  SHOW is called when a command is going to be edited.      *
 *  Create and initialize the appropriate dialog box.         *
 *                                                            *
 *  SHOW functions recieve the following variables            *
 *     HWND container - window to draw our dialog box inside  *
 *     int  id      - id of the function to execute           *
 *     char *params - user-specified command parameters       *
 *                                                            *
\**************************************************************/

SHOW(ShowComboParam) {
   // Create the dialog box inside the containing window
   DIALOG(DLG_COMBO, DlgProc);

   // match the command, and set the description appropraitely
   if(id == pEditFile->id) {
		SET(IDC_COMMAND, STR(EDITFILE_DESC));
		SET(IDC_VAL1_DESC, STR(EDIT_SELECT));
		BEGIN_RESTORE();
		COMBO(IDC_COMBO, STR(EDIT_STYLE), "0");
		COMBO(IDC_COMBO, "menu.rc", "1");
		COMBO(IDC_COMBO, "plugins.rc", "2");
		COMBO(IDC_COMBO, "extensions.rc", "3");
		COMBO(IDC_COMBO, "blackbox.rc", "4");
		RestoreCombo(IDC_COMBO);
		END_RESTORE();
   } else if(id == pShowMenu->id) {
		SET(IDC_COMMAND, STR(SHOWMENU_DESC));
		SET(IDC_VAL1_DESC, STR(MENU_SELECT));
		BEGIN_RESTORE();
		COMBO(IDC_COMBO, STR(MENU_MAIN), "0");
		COMBO(IDC_COMBO, STR(MENU_WORKSPACE), "1");
		COMBO(IDC_COMBO, STR(MENU_TOOLBAR), "2");
		COMBO(IDC_COMBO, STR(MENU_SYSTEMBAR), "3");
		COMBO(IDC_COMBO, STR(MENU_SLIT), "4");
		RestoreCombo(IDC_COMBO);
		END_RESTORE();
   } else if(id == pWorkSpace->id) {
		SET(IDC_COMMAND, STR(WORKSPACE_DESC));
		SET(IDC_VAL1_DESC, STR(WORKSPACE_SELECT));
		BEGIN_RESTORE();
		COMBO(IDC_COMBO, STR(WORKSPACE_LEFT), "0");
		COMBO(IDC_COMBO, STR(WORKSPACE_RIGHT), "1");
		COMBO(IDC_COMBO, STR(WORKSPACE_ADD), "2");
		COMBO(IDC_COMBO, STR(WORKSPACE_DEL), "3");
		COMBO(IDC_COMBO, STR(WORKSPACE_GATHER), "5");
		COMBO(IDC_COMBO, STR(WORKSPACE_MOVELEFT), "6");
		COMBO(IDC_COMBO, STR(WORKSPACE_MOVERIGHT), "7");
		RestoreCombo(IDC_COMBO);
		END_RESTORE();
   }
   SHOW_RETURN();
}

SHOW(ShowEditParam)
{
	DIALOG(DLG_EDIT, DlgProc);
	SET(IDC_COMMAND, STR(BROAM_DESC));
	SET(IDC_VAL1_DESC, STR(BROAM_EDIT));
	BEGIN_RESTORE();
	RestoreItem(IDC_EDIT);
	END_RESTORE();
	SHOW_RETURN();
}

/*****************************************************************\
 *                                                               *
 *  SAVE is called when the config dialog is about to be closed  *
 *                                                               *
 *     HWND  hWnd    - handle to the dialog box created in SHOW  *
 *     int   id      - id of the function to save                *
 *     int  *lenght  - the length of the returned string         *
 *     int  *lpszBuf - the buffer to store the returned string   *
 *     void *data    - pointer to any data previously            *
 *                     associated with this command              *
 *                                                               *
 *  If lpszBuf is not null, copy the text parameters into it.    *
 *  Always set lenght to the lenght of the text parameters.      *
 *  Always return a pointer to the data associated with this     *
 *  command.  If you are not associating data, return NULL       *
 *                                                               *
\*****************************************************************/

SAVE(SaveComboParam) {
   PARAMS(16);    // the maximum length of the parameter string
   SaveCombo(IDC_COMBO);
   SAVEPARAMS();
}

SAVE(SaveEditParam) {
	PARAMS(1024);
	SaveItem(IDC_EDIT);
	SAVEPARAMS();
}

/***********************************************************\
 *                                                         *
 *  CLEAR is called when a command associated with this    *
 *  plugin is about to be destroyed.  This is your chance  *
 *  to delete any data you have stored in the command      *
 *                                                         *
 *  CLEAR functions recieve only one parameter, data,      *
 *  which contains a pointer to any data associated with   *
 *  this command.                                          *
 *                                                         *
\***********************************************************/

CLEAR(ClearComboParam) {
   delete data;
}

CLEAR(ClearEditParam) {
	delete data;
}

/**************************************************\
 *                                                *
 *  Quit is called before the plugin is unloaded  *
 *  This is the place to do you code cleanup      *
 *                                                *
\**************************************************/

void Quit() {
   DeleteCommands();    // remove the command list
}

BOOL CALLBACK DlgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
   switch (uMsg) {
      case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		default:
			return FALSE;
   }
   return TRUE;
}
