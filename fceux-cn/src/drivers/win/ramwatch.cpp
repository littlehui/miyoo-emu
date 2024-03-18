#include <windows.h>
#include "common.h"
#include "main.h"
using namespace std;

#include "resource.h"
#include "cheat.h"
#include "ramwatch.h"
#include "ram_search.h"
#include <assert.h>
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <map>
#include <bitset>

/*
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")
#include <commdlg.h>
#pragma comment(lib, "comdlg32.lib")
*/

static HMENU ramwatchmenu;
static HMENU rwrecentmenu;
/*static*/ HACCEL RamWatchAccels = NULL;
char rw_recent_files[MAX_RECENT_WATCHES][1024] = {"","","","",""};
//char Watch_Dir[1024]="";
bool RWfileChanged = false; //Keeps track of whether the current watch file has been changed, if so, ramwatch will prompt to save changes
bool AutoRWLoad = false;    //Keeps track of whether Auto-load is checked
bool RWSaveWindowPos = false; //Keeps track of whether Save Window position is checked
char currentWatch[1024];
int ramw_x, ramw_y;			//Used to store ramwatch dialog window positions
std::map<int, AddressWatcher> rswatches;
int WatchCount = 0;

char applicationPath[2048];
struct InitRamWatch
{
	InitRamWatch()
	{
		GetModuleFileName(NULL, applicationPath, 2048);
	}
} initRamWatch;

HPEN SeparatorCache::sepPen = NULL;
HPEN SeparatorCache::sepPenSel = NULL;
HFONT SeparatorCache::sepFon = NULL;
int SeparatorCache::iHeight = 0;
int SeparatorCache::sepOffY = 0;

std::map<int, SeparatorCache> separatorCache;

HWND RamWatchHWnd;
#define gamefilename GetRomName()
#define hWnd hAppWnd
#define hInst fceu_hInstance
// static char Str_Tmp [1024];

void init_list_box(HWND Box, const char* Strs[], int numColumns, int *columnWidths); //initializes the ram search and/or ram watch listbox

#define MESSAGEBOXPARENT (RamWatchHWnd ? RamWatchHWnd : hWnd)

bool QuickSaveWatches();
bool ResetWatches();

void RefreshWatchListSelectedCountControlStatus(HWND hDlg, int newComer = -1);

unsigned int GetCurrentValue(AddressWatcher& watch)
{
	//TODO: A similar if for 4-byte just to be through, but there shouldn't be any reason to have 4-byte on the NES!
	if (watch.Size == 'w')	//Do little endian
	{
		unsigned int x = ReadValueAtHardwareAddress(watch.Address+1, 1) * 256;
		x += ReadValueAtHardwareAddress(watch.Address, 1);
		return x;
	}
	else
		return ReadValueAtHardwareAddress(watch.Address, watch.Size == 'd' ? 4 : watch.Size == 'w' ? 2 : 1);
}

bool IsSameWatch(const AddressWatcher& l, const AddressWatcher& r)
{
	if (r.Size == 'S') return false;
	return ((l.Address == r.Address) && (l.Size == r.Size) && (l.Type == r.Type)/* && (l.WrongEndian == r.WrongEndian)*/);
}

bool VerifyWatchNotAlreadyAdded(const AddressWatcher& watch)
{
	for (int j = 0; j < WatchCount; j++)
	{
		if (IsSameWatch(rswatches[j], watch))
		{
			if(RamWatchHWnd)
				SetForegroundWindow(RamWatchHWnd);
			return false;
		}
	}
	return true;
}


bool InsertWatch(const AddressWatcher& Watch)
{
	if(!VerifyWatchNotAlreadyAdded(Watch))
		return false;

	if(WatchCount >= MAX_WATCH_COUNT)
		return false;

	int i = WatchCount++;
	AddressWatcher& NewWatch = rswatches[i];
	NewWatch = Watch;

	NewWatch.CurValue = GetCurrentValue(NewWatch);
	extern int FCEU_CalcCheatAffectedBytes(uint32, uint32);
	NewWatch.Cheats = FCEU_CalcCheatAffectedBytes(NewWatch.Address, WatchSizeConv(NewWatch));
	NewWatch.comment = strcpy((char*)malloc(strlen(Watch.comment) + 2), Watch.comment);

	// currently it's impossible to add a separator from outside RAM Watch, so no need to check the handle 
	if (NewWatch.Type == 'S')
		separatorCache[i] = SeparatorCache(RamWatchHWnd, NewWatch.comment);

	// In case the window is not open and somebody call this method outside.
	if (RamWatchHWnd)
		ListView_SetItemCount(GetDlgItem(RamWatchHWnd, IDC_WATCHLIST), WatchCount);
	RWfileChanged = true;

	return true;
}

bool InsertWatch(const AddressWatcher& Watch, HWND parent)
{
	if(!VerifyWatchNotAlreadyAdded(Watch))
		return false;

	if(!parent)
		parent = RamWatchHWnd;
	if(!parent)
		parent = hWnd;

	int prevWatchCount = WatchCount;

	int tmpWatchIndex;
	if (parent == RamWatchHWnd)
		tmpWatchIndex = WatchCount;
	else if (parent == RamSearchHWnd)
		tmpWatchIndex = -2;
	else if (parent == hCheat)
		tmpWatchIndex = -3;
	else
		tmpWatchIndex = -4;

	rswatches[tmpWatchIndex] = Watch;
	rswatches[tmpWatchIndex].CurValue = GetCurrentValue(rswatches[tmpWatchIndex]);
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EDITWATCH), parent, EditWatchProc, tmpWatchIndex);
	rswatches.erase(tmpWatchIndex);

	return WatchCount > prevWatchCount;
}

bool InsertWatches(const AddressWatcher* watches, HWND parent, const int count)
{
	if (count == 1)
		return InsertWatch(watches[0], parent);
	else
	{
		bool success = false;
		char comment[256];
		rswatches[-1] = watches[0];
		rswatches[-1].comment = comment;
		if(DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EDITWATCH), parent, EditWatchProc, (LPARAM)-1))
			for (int i = 0; i < count; ++i)
			{
				AddressWatcher watcher = watches[i];
				watcher.comment = rswatches[-1].comment;
				success |= InsertWatch(watcher);
			}
		rswatches.erase(-1);
		return success;
	}
	return false;
}

void Update_RAM_Watch()
{
	BOOL watchChanged[MAX_WATCH_COUNT] = {0};

	if(WatchCount)
	{
		// update cached values and detect changes to displayed listview items

		for(int i = 0; i < WatchCount; i++)
		{
			if (rswatches[i].Type != 'S')
			{
				unsigned int prevCurValue = rswatches[i].CurValue;
				unsigned int newCurValue = GetCurrentValue(rswatches[i]);
				if (prevCurValue != newCurValue)
				{
					rswatches[i].CurValue = newCurValue;
					watchChanged[i] = TRUE;
				}
			}
		}
	}

	// refresh any visible parts of the listview box that changed
	HWND lv = GetDlgItem(RamWatchHWnd,IDC_WATCHLIST);
	int top = ListView_GetTopIndex(lv);
	int bottom = top + ListView_GetCountPerPage(lv) + 1; // +1 is so we will update a partially-displayed last item
	if(top < 0) top = 0;
	if(bottom > WatchCount) bottom = WatchCount;
	int start = -1;
	for(int i = top; i <= bottom; i++)
	{
		if(start == -1)
		{
			if(i != bottom && watchChanged[i])
			{
				start = i;
				//somethingChanged = true;
			}
		}
		else
		{
			if(i == bottom || !watchChanged[i])
			{
				ListView_RedrawItems(lv, start, i-1);
				start = -1;
			}
		}
	}
}

bool AskSaveRamWatch()
{
	//This function asks to save changes if the watch file contents have changed
	//returns false only if a save was attempted but failed or was cancelled
	if (RWfileChanged)
	{
		int answer = MessageBox(MESSAGEBOXPARENT, "Save Changes?", "Ram Watch", MB_YESNOCANCEL | MB_ICONQUESTION);
		if(answer == IDYES)
			if(!QuickSaveWatches())
				return false;
		return (answer != IDCANCEL);
	}
	return true;
}

void WriteRecentRWFiles()
{
	char str[2048];
	for (int i = 0; i < MAX_RECENT_WATCHES; i++)
	{
		sprintf(str, "recentWatch%d", i+1);
		//regSetStringValue(str, &rw_recent_files[i][0]); TODO
	}
}

void UpdateRW_RMenu(HMENU menu, unsigned int mitem, unsigned int baseid)
{
	MENUITEMINFO moo;
	int x;

	moo.cbSize = sizeof(moo);
	moo.fMask = MIIM_SUBMENU | MIIM_STATE;

	GetMenuItemInfo(GetSubMenu(ramwatchmenu, 0), mitem, FALSE, &moo);
	moo.hSubMenu = menu;
	moo.fState = strlen(rw_recent_files[0]) ? MFS_ENABLED : MFS_GRAYED;

	SetMenuItemInfo(GetSubMenu(ramwatchmenu, 0), mitem, FALSE, &moo);

	// Remove all recent files submenus
	for(x = 0; x < MAX_RECENT_WATCHES; x++)
	{
		RemoveMenu(menu, baseid + x, MF_BYCOMMAND);
	}

	// Recreate the menus
	for(x = MAX_RECENT_WATCHES - 1; x >= 0; x--)
	{  
		char tmp[128 + 5];

		// Skip empty strings
		if(!strlen(rw_recent_files[x]))
		{
			continue;
		}

		moo.cbSize = sizeof(moo);
		moo.fMask = MIIM_DATA | MIIM_ID | MIIM_TYPE;

		// Fill in the menu text.
		if(strlen(rw_recent_files[x]) < 128)
		{
			sprintf(tmp, "&%d. %s", ( x + 1 ) % 10, rw_recent_files[x]);
		}
		else
		{
			sprintf(tmp, "&%d. %s", ( x + 1 ) % 10, rw_recent_files[x] + strlen( rw_recent_files[x] ) - 127);
		}

		// Insert the menu item
		moo.cch = strlen(tmp);
		moo.fType = 0;
		moo.wID = baseid + x;
		moo.dwTypeData = tmp;
		InsertMenuItem(menu, 0, 1, &moo);
	}

	// I don't think one function shall do so many things in a row
//	WriteRecentRWFiles();	// write recent menu to ini
}

void UpdateRWRecentArray(const char* addString, unsigned int arrayLen, HMENU menu, unsigned int menuItem, unsigned int baseId)
{
	const size_t len = 1024; // Avoid magic numbers

	// Try to find out if the filename is already in the recent files list.
	for(unsigned int x = 0; x < arrayLen; x++)
	{
		if(strlen(rw_recent_files[x]))
		{
			if(!strncmp(rw_recent_files[x], addString, 1024))    // Item is already in list.
			{
				// If the filename is in the file list don't add it again.
				// Move it up in the list instead.

				int y;
				char tmp[len];

				// Save pointer.
				strncpy(tmp, rw_recent_files[x], len);
				
				for(y = x; y; y--)
				{
					// Move items down.
					strncpy(rw_recent_files[y],rw_recent_files[y - 1], len);
				}

				// Put item on top.
				strncpy(rw_recent_files[0],tmp, len);

				// Update the recent files menu
				UpdateRW_RMenu(menu, menuItem, baseId);

				return;
			}
		}
	}

	// The filename wasn't found in the list. That means we need to add it.

	// Move the other items down.
	for(unsigned int x = arrayLen - 1; x; x--)
	{
		strncpy(rw_recent_files[x],rw_recent_files[x - 1], len);
	}

	// Add the new item.
	strncpy(rw_recent_files[0], addString, len);

	// Update the recent files menu
	UpdateRW_RMenu(menu, menuItem, baseId);
}

void RWAddRecentFile(const char *filename)
{
	UpdateRWRecentArray(filename, MAX_RECENT_WATCHES, rwrecentmenu, RAMMENU_FILE_RECENT, RW_MENU_FIRST_RECENT_FILE);
}

void OpenRWRecentFile(int memwRFileNumber)
{
	if(!ResetWatches())
		return;

	int rnum = memwRFileNumber;
	if ((unsigned int)rnum >= MAX_RECENT_WATCHES)
		return; //just in case

	char* x;

	while(true)
	{
		x = rw_recent_files[rnum];
		if (!*x) 
			return;		//If no recent files exist just return.  Useful for Load last file on startup (or if something goes screwy)

		if (rnum) //Change order of recent files if not most recent
		{
			RWAddRecentFile(x);
			rnum = 0;
		}
		else
		{
			break;
		}
	}

	char Str_Tmp[1024];
	strcpy(currentWatch,x);
	strcpy(Str_Tmp,currentWatch);

	//loadwatches here
	FILE *WatchFile = fopen(Str_Tmp,"rb");
	if (!WatchFile)
	{
		int answer = MessageBox(MESSAGEBOXPARENT,"Error opening file.","Ram Watch",MB_OKCANCEL | MB_ICONERROR);
		if (answer == IDOK)
		{
			rw_recent_files[rnum][0] = '\0';	//Clear file from list 
			if (rnum)							//Update the ramwatch list
				RWAddRecentFile(rw_recent_files[0]); 
			else
				RWAddRecentFile(rw_recent_files[1]);
		}
		return;
	}
	const char DELIM = '\t';
	AddressWatcher Temp;
	Temp.Address = 0;	// default values
	Temp.Size = 'b';
	Temp.Type = 'h';
	Temp.comment = NULL;
	char mode;
	fgets(Str_Tmp,1024,WatchFile);
	sscanf(Str_Tmp,"%c%*s",&mode);
	int WatchAdd;
	fgets(Str_Tmp,1024,WatchFile);
	sscanf(Str_Tmp,"%d%*s",&WatchAdd);
	WatchAdd+=WatchCount;
	for (int i = WatchCount; i < WatchAdd; i++)
	{
		if (i < 0) i = 0;
		memset(Str_Tmp, 0, 1024);
		do
		{
			fgets(Str_Tmp, 1024, WatchFile);
		} while (Str_Tmp[0] == '\n');
		sscanf(Str_Tmp, "%*05X%*c%04X%*c%c%*c%c%*c%*c", &(Temp.Address), &(Temp.Size), &(Temp.Type));
		Temp.WrongEndian = false;
		char *Comment = strrchr(Str_Tmp, DELIM);
		if (Comment)
		{
			Comment++;
			char *CommentEnd = strrchr(Comment, '\n');
			if (CommentEnd)
			{
				*CommentEnd = '\0';
				Temp.comment = Comment;
				InsertWatch(Temp);
			} else
			{
				// the wch file is probably corrupted
				Temp.comment = Comment;
				InsertWatch(Temp);
				break;
			}
		} else
			break;	// the wch file is probably corrupted
	}

	fclose(WatchFile);
	if (RamWatchHWnd) {
		ListView_SetItemCount(GetDlgItem(RamWatchHWnd,IDC_WATCHLIST),WatchCount);
		RefreshWatchListSelectedCountControlStatus(RamWatchHWnd);
	}
	RWfileChanged=false;
	return;
}

int Change_File_L(char *Dest, const char *Dir, const char *Titre, const char *Filter, const char *Ext, HWND hwnd)
{
	OPENFILENAME ofn;

	SetCurrentDirectory(applicationPath);

	if (!strcmp(Dest, ""))
	{
		strcpy(Dest, "default.");
		strcat(Dest, Ext);
	}

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = hInst;
	ofn.lpstrFile = Dest;
	ofn.nMaxFile = 2047;
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = Dir;
	ofn.lpstrTitle = Titre;
	ofn.lpstrDefExt = Ext;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (GetOpenFileName(&ofn)) return 1;

	return 0;
}

int Change_File_S(char *Dest, const char *Dir, const char *Titre, const char *Filter, const char *Ext, HWND hwnd)
{
	OPENFILENAME ofn;

	SetCurrentDirectory(applicationPath);

	if (!strcmp(Dest, ""))
	{
		strcpy(Dest, "default.");
		strcat(Dest, Ext);
	}

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = hInst;
	ofn.lpstrFile = Dest;
	ofn.nMaxFile = 2047;
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = Dir;
	ofn.lpstrTitle = Titre;
	ofn.lpstrDefExt = Ext;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	if (GetSaveFileName(&ofn)) {
		return 1;
	}

	return 0;
}

bool Save_Watches()
{
	char Str_Tmp[1024];
	string initdir = FCEU_GetPath(FCEUMKF_MEMW);
	const char* slash = max(strrchr(gamefilename, '|'), max(strrchr(gamefilename, '\\'), strrchr(gamefilename, '/')));
	strcpy(Str_Tmp,slash ? slash+1 : gamefilename);
	char* dot = strrchr(Str_Tmp, '.');
	if(dot) *dot = 0;
	if(Change_File_S(Str_Tmp, initdir.c_str(), "Save Watches", "Watchlist (*.wch)\0*.wch\0All Files (*.*)\0*.*\0\0", "wch", RamWatchHWnd))
	{
		FILE *WatchFile = fopen(Str_Tmp,"w+b");
		fputc('\n',WatchFile);
		strcpy(currentWatch,Str_Tmp);
		RWAddRecentFile(currentWatch);
		sprintf(Str_Tmp,"%d\n",WatchCount);
		fputs(Str_Tmp,WatchFile);
		const char DELIM = '\t';
		for (int i = 0; i < WatchCount; i++)
		{
			sprintf(Str_Tmp,"%05X%c%04X%c%c%c%c%c%d%c%s\n",i,DELIM,rswatches[i].Address,DELIM,rswatches[i].Size,DELIM,rswatches[i].Type,DELIM,rswatches[i].WrongEndian,DELIM,rswatches[i].comment);
			fputs(Str_Tmp,WatchFile);
		}
		
		fclose(WatchFile);
		RWfileChanged=false;
		//TODO: Add to recent list function call here
		return true;
	}
	return false;
}

bool QuickSaveWatches()
{
	if (RWfileChanged==false) return true; //If file has not changed, no need to save changes
		if (currentWatch[0] == NULL) //If there is no currently loaded file, run to Save as and then return
			return Save_Watches();
	
	char Str_Tmp[1024];
	strcpy(Str_Tmp,currentWatch);
	FILE *WatchFile = fopen(Str_Tmp,"w+b");
	fputc('\n',WatchFile);
	sprintf(Str_Tmp,"%d\n",WatchCount);
	fputs(Str_Tmp,WatchFile);
	const char DELIM = '\t';
	for (int i = 0; i < WatchCount; i++)
	{
		sprintf(Str_Tmp,"%05X%c%04X%c%c%c%c%c%d%c%s\n",i,DELIM,rswatches[i].Address,DELIM,rswatches[i].Size,DELIM,rswatches[i].Type,DELIM,rswatches[i].WrongEndian,DELIM,rswatches[i].comment);
		fputs(Str_Tmp,WatchFile);
	}
	fclose(WatchFile);
	RWfileChanged=false;
	return true;
}

bool Load_Watches(bool clear, const char* filename)
{
	const char DELIM = '\t';
	FILE* WatchFile = fopen(filename,"rb");
	if (!WatchFile)
	{
		MessageBox(MESSAGEBOXPARENT,"Error opening file.","Ram Watch",MB_OK | MB_ICONERROR);
		return false;
	}
	if(clear)
	{
		if(!ResetWatches())
		{
			fclose(WatchFile);
			return false;
		}
	}
	strcpy(currentWatch,filename);
	RWAddRecentFile(currentWatch);
	AddressWatcher Temp;
	Temp.Address = 0;	// default values
	Temp.Size = 'b';
	Temp.Type = 'h';
	char mode;
	char Str_Tmp[1024];
	fgets(Str_Tmp,1024,WatchFile);
	sscanf(Str_Tmp,"%c%*s",&mode);
	int WatchAdd;
	fgets(Str_Tmp,1024,WatchFile);
	sscanf(Str_Tmp,"%d%*s",&WatchAdd);
	WatchAdd+=WatchCount;
	for (int i = WatchCount; i < WatchAdd; i++)
	{
		if (i < 0) i = 0;
		memset(Str_Tmp, 0, 1024);
		do
		{
			fgets(Str_Tmp, 1024, WatchFile);
		} while (Str_Tmp[0] == '\n');
		sscanf(Str_Tmp, "%*05X%*c%04X%*c%c%*c%c%*c%*c", &(Temp.Address), &(Temp.Size), &(Temp.Type));
		Temp.WrongEndian = false;
		char *Comment = strrchr(Str_Tmp, DELIM);
		if (Comment)
		{
			Comment++;
			char *CommentEnd = strrchr(Comment, '\n');
			if (CommentEnd)
			{
				*CommentEnd = '\0';
				Temp.comment = Comment;
				InsertWatch(Temp);
			} else
			{
				// the wch file is probably corrupted
				Temp.comment = Comment;
				InsertWatch(Temp);
				break;
			}
		} else
			break;	// the wch file is probably corrupted
	}
	
	fclose(WatchFile);
	if (RamWatchHWnd)
		ListView_SetItemCount(GetDlgItem(RamWatchHWnd,IDC_WATCHLIST),WatchCount);
	RWfileChanged=false;
	return true;
}

bool Load_Watches(bool clear)
{
	char Str_Tmp[1024];
	string initdir = FCEU_GetPath(FCEUMKF_MEMW);
	const char* slash = max(strrchr(gamefilename, '|'), max(strrchr(gamefilename, '\\'), strrchr(gamefilename, '/')));
	strcpy(Str_Tmp,slash ? slash+1 : gamefilename);
	char* dot = strrchr(Str_Tmp, '.');
	if(dot) *dot = 0;
	strcat(Str_Tmp,".wch");
	if(Change_File_L(Str_Tmp, initdir.c_str(), "Load Watches", "Watchlist (*.wch)\0*.wch\0All Files (*.*)\0*.*\0\0", "wch", RamWatchHWnd))
	{
		return Load_Watches(clear, Str_Tmp);
	}
	return false;
}

bool ResetWatches()
{
	if(!AskSaveRamWatch())
		return false;
	for (;WatchCount >= 0; WatchCount--)
	{
		free(rswatches[WatchCount].comment);
		rswatches[WatchCount].comment = NULL;
	}
	rswatches.clear();
	WatchCount = 0;
	if (RamWatchHWnd)
	{
		ListView_SetItemCount(GetDlgItem(RamWatchHWnd,IDC_WATCHLIST),WatchCount);
		RefreshWatchListSelectedCountControlStatus(RamWatchHWnd);
	}
	RWfileChanged = false;
	currentWatch[0] = NULL;

	separatorCache.clear();
	return true;
}

bool RemoveWatch(int watchIndex)
{
	AddressWatcher& target = rswatches[watchIndex];
	if (watchIndex >= WatchCount)
		return false;
	if (target.comment)
	{
		free(target.comment);
		target.comment = NULL;
	}
	if (target.Type == 'S')
		separatorCache.erase(watchIndex);
	rswatches.erase(watchIndex);
	for (int i = watchIndex; i <= WatchCount; i++)
	{
		rswatches[i] = rswatches[i + 1];
		separatorCache[i] = separatorCache[i + 1];
	}
	WatchCount--;
	return true;
}

bool EditWatch(int watchIndex, AddressWatcher& watcher)
{
	if (watchIndex >= WatchCount)
		return InsertWatch(watcher);

	AddressWatcher& original = rswatches[watchIndex];

	if (watcher.Type != 'S' && VerifyWatchNotAlreadyAdded(watcher))
	{

		if (original.Address != watcher.Address || original.Size != watcher.Size)
		{
			extern int FCEU_CalcCheatAffectedBytes(uint32, uint32);
			original.Cheats = FCEU_CalcCheatAffectedBytes(watcher.Address, WatchSizeConv(watcher));
			original.CurValue = GetCurrentValue(watcher);
		}

		original.Address = watcher.Address;
		original.Type = watcher.Type;
		original.Size = watcher.Size;
		original.WrongEndian = watcher.WrongEndian;

		RWfileChanged = true;
	}

	if (strcmp(original.comment, watcher.comment))
	{
		if (original.comment)
			original.comment = strcpy((char*)realloc(original.comment, strlen(watcher.comment) + 2), watcher.comment);
		else
			original.comment = strcpy((char*)malloc(strlen(watcher.comment) + 2), watcher.comment);

		if (original.Type == 'S')
			separatorCache[watchIndex] = SeparatorCache(RamWatchHWnd, watcher.comment);

		return true;
	}

	return false;
}

void RefreshWatchListSelectedItemControlStatus(HWND hDlg)
{
	static int prevSelIndex=-1;
	int selIndex = ListView_GetSelectionMark(GetDlgItem(hDlg,IDC_RAMLIST));
	if(selIndex != prevSelIndex)
	{
		if(selIndex == -1 || prevSelIndex == -1)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_C_ADDCHEAT), (selIndex != -1) ? TRUE : FALSE);
		}
		prevSelIndex = selIndex;
	}
}

INT_PTR CALLBACK EditWatchProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) //Gets info for a RAM Watch, and then inserts it into the Watch List
{

	// since there are 3 windows can pops up the add watch dialog, we should store them separately.
	// 0 for ram watch, 1 for ram search, 2 for cheat dialog.
	static int indexes[4];

	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				RECT r;
				GetWindowRect(hWnd, &r);
				SetWindowPos(hDlg, NULL, r.left, r.top, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
				
				AddressWatcher& watcher = rswatches[lParam];
				if (watcher.Type != 'S') {
					char Str_Tmp[1024];
					// -1 means batch add
					if (lParam != -1)
					{
						sprintf(Str_Tmp, "%04X", watcher.Address);
						SetDlgItemText(hDlg, IDC_EDIT_COMPAREADDRESS, Str_Tmp);
					} else
						// Add multiple watches
						SetDlgItemText(hDlg, IDC_EDIT_COMPAREADDRESS, "(multiple)");

					switch (watcher.Size)
					{
						case 'b':
							SendDlgItemMessage(hDlg, IDC_1_BYTE, BM_SETCHECK, BST_CHECKED, 0);
							break;
						case 'w':
							SendDlgItemMessage(hDlg, IDC_2_BYTES, BM_SETCHECK, BST_CHECKED, 0);
							break;
						case 'd':
							SendDlgItemMessage(hDlg, IDC_4_BYTES, BM_SETCHECK, BST_CHECKED, 0);
							break;
					}
					switch (watcher.Type)
					{
						case 's':
							SendDlgItemMessage(hDlg, IDC_SIGNED, BM_SETCHECK, BST_CHECKED, 0);
							break;
						case 'u':
							SendDlgItemMessage(hDlg, IDC_UNSIGNED, BM_SETCHECK, BST_CHECKED, 0);
							break;
						case 'h':
							SendDlgItemMessage(hDlg, IDC_HEX, BM_SETCHECK, BST_CHECKED, 0);
							break;
						case 'b':
							SendDlgItemMessage(hDlg, IDC_BINARY, BM_SETCHECK, BST_CHECKED, 0);
							break;
					}
				} else
					SetDlgItemText(hDlg, IDC_EDIT_COMPAREADDRESS, "---------");

				if (watcher.comment != NULL)
					SetDlgItemText(hDlg, IDC_PROMPT_EDIT, watcher.comment);

				HWND parent = GetParent(hDlg);
				indexes[GetDlgStoreIndex(parent)] = lParam;

				if (watcher.Type == 'S' || parent == RamSearchHWnd || parent == hCheat)
				{
					EnableWindow(GetDlgItem(hDlg, IDC_SPECIFICADDRESS), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_DATATYPE_GROUPBOX), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_DATASIZE_GROUPBOX), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_EDIT_COMPAREADDRESS), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_SIGNED), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_UNSIGNED), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_HEX), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_BINARY), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_1_BYTE), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_2_BYTES), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_4_BYTES), FALSE);
					SetFocus(GetDlgItem(hDlg, IDC_PROMPT_EDIT));
				}
			}

			return true;
			break;
		
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
				{
					char Str_Tmp[256];
					
					HWND parent = GetParent(hDlg);
					int index = indexes[GetDlgStoreIndex(parent)];

					// not a single watch editing operation
					if (index != -1)
					{
						// a normal watch, copy it to a temporary one
						AddressWatcher watcher = rswatches[index];
						// if (watcher.comment != NULL)
						//	 watcher.comment = strcpy((char*)malloc(strlen(watcher.comment) + 2), watcher.comment);

						// It's from ram watch window, not a separator
						// When it's from ram search or cheat window, all the information required is already set,
						// so this is also unecessary
						if (RamWatchHWnd && RamWatchHWnd == GetParent(hDlg) && watcher.Type != 'S')
						{
							GetDlgItemText(hDlg, IDC_PROMPT_EDIT, Str_Tmp, 256);

							// type
							if (SendDlgItemMessage(hDlg, IDC_SIGNED, BM_GETCHECK, 0, 0) == BST_CHECKED)
								watcher.Type = 's';
							else if (SendDlgItemMessage(hDlg, IDC_UNSIGNED, BM_GETCHECK, 0, 0) == BST_CHECKED)
								watcher.Type = 'u';
							else if (SendDlgItemMessage(hDlg, IDC_HEX, BM_GETCHECK, 0, 0) == BST_CHECKED)
								watcher.Type = 'h';
							else if (SendDlgItemMessage(hDlg, IDC_BINARY, BM_GETCHECK, 0, 0) == BST_CHECKED)
								watcher.Type = 'b';
							else {
								MessageBox(hDlg, "Type must be specified.", "Error", MB_OK | MB_ICONERROR);
								return true;
							}

							// size
							if (SendDlgItemMessage(hDlg, IDC_1_BYTE, BM_GETCHECK, 0, 0) == BST_CHECKED)
								watcher.Size = 'b';
							else if (SendDlgItemMessage(hDlg, IDC_2_BYTES, BM_GETCHECK, 0, 0) == BST_CHECKED)
								watcher.Size = 'w';
							else if (SendDlgItemMessage(hDlg, IDC_4_BYTES, BM_GETCHECK, 0, 0) == BST_CHECKED)
								watcher.Size = 'd';
							else {
								MessageBox(hDlg, "Size must be specified.", "Error", MB_OK | MB_ICONERROR);
								return true;
							}

							if (watcher.Type == 'b' && (watcher.Size == 'd' || watcher.Size == 'w'))
							{
								MessageBox(hDlg, "Only 1 byte is supported on binary format.", "Error", MB_OK | MB_ICONERROR);
								return true;
							}

							// address
							GetDlgItemText(hDlg, IDC_EDIT_COMPAREADDRESS, Str_Tmp, 1024);
							char *addrstr = Str_Tmp;
							if (strlen(Str_Tmp) > 8)
								addrstr = &Str_Tmp[strlen(Str_Tmp) - 9];
							for (int i = 0; addrstr[i]; ++i)
								if (toupper(addrstr[i]) == 'O')
									addrstr[i] = '0';
							sscanf(addrstr, "%04X", &watcher.Address);

							if ((watcher.Address & ~0xFFFFFF) == ~0xFFFFFF)
								watcher.Address &= 0xFFFFFF;

							if (!IsHardwareAddressValid(watcher.Address))
							{
								MessageBox(hDlg, "Invalid Address.", "Error", MB_OK | MB_ICONERROR);
								return true;
							}
						}

						// comment
						GetDlgItemText(hDlg, IDC_PROMPT_EDIT, Str_Tmp, 80);
						watcher.comment = Str_Tmp;

						// finallly update the watch list
						if (index >= 0 && index < WatchCount)
							// it's a watch editing operation.
							// Only ram watch window can edit a watch, the ram search window and cheat window only add watch.
							EditWatch(index, watcher);
						else
							InsertWatch(watcher);
						if (RamWatchHWnd)
							ListView_SetItemCount(GetDlgItem(RamWatchHWnd, IDC_WATCHLIST), WatchCount);
					}
					else {
						// a multiple watches insert operation, just asking for a comment
						AddressWatcher& watcher = rswatches[index];
						// comment
						GetDlgItemText(hDlg, IDC_PROMPT_EDIT, Str_Tmp, 80);
						strcpy(watcher.comment, Str_Tmp);
					}
					EndDialog(hDlg, true);

					RWfileChanged = true;
					return true;
					break;
				}
				case IDCANCEL:
					EndDialog(hDlg, false);
					return false;
					break;
			}
			break;

		case WM_CLOSE:
			EndDialog(hDlg, false);
			return false;
			break;
	}

	return false;
}




void RamWatchEnableCommand(HWND hDlg, HMENU hMenu, UINT uIDEnableItem, bool enable)
{
	EnableWindow(GetDlgItem(hDlg, uIDEnableItem), enable ? TRUE : FALSE);
	if (hMenu != NULL) {
		if (uIDEnableItem == ID_WATCHES_UPDOWN) {
			EnableMenuItem(hMenu, IDC_C_WATCH_UP, MF_BYCOMMAND | enable ? MF_ENABLED : MF_GRAYED | MF_DISABLED);
			EnableMenuItem(hMenu, IDC_C_WATCH_DOWN, MF_BYCOMMAND | enable ? MF_ENABLED : MF_GRAYED | MF_DISABLED);
		}
		else
			EnableMenuItem(hMenu, uIDEnableItem, MF_BYCOMMAND | enable ? MF_ENABLED : MF_GRAYED | MF_DISABLED);
	}
}

void RefreshWatchListSelectedCountControlStatus(HWND hDlg, int newComer)
{
	static int prevSelCount=-1;
	int selCount = ListView_GetSelectedCount(GetDlgItem(hDlg,IDC_WATCHLIST));

	// int sel = SendDlgItemMessage(hDlg, IDC_WATCHLIST, LVM_GETSELECTIONMARK, 0, 0);
	if(selCount != prevSelCount)
	{
		if(selCount < 2 || prevSelCount < 2)
		{
			RamWatchEnableCommand(hDlg, ramwatchmenu, IDC_C_WATCH_EDIT, selCount == 1);
			RamWatchEnableCommand(hDlg, ramwatchmenu, IDC_C_WATCH_REMOVE, selCount >= 1);
			RamWatchEnableCommand(hDlg, ramwatchmenu, IDC_C_WATCH_DUPLICATE, selCount == 1);
			RamWatchEnableCommand(hDlg, ramwatchmenu, ID_WATCHES_UPDOWN, selCount == 1);
			RamWatchEnableCommand(hDlg, ramwatchmenu, IDC_C_ADDCHEAT, selCount == 1 && newComer != -1 && rswatches[newComer].Type != 'S');
		}
		prevSelCount = selCount;
	}
}

INT_PTR CALLBACK RamWatchProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int watchIndex = 0; // current watch index
	static bool listFocus;

	switch(uMsg)
	{
		case WM_MOVE: {
			if (!IsIconic(hDlg)) {
			RECT wrect;
			GetWindowRect(hDlg,&wrect);
			ramw_x = wrect.left;
			ramw_y = wrect.top;

			#ifdef WIN32
			WindowBoundsCheckNoResize(ramw_x,ramw_y,wrect.right);
			#endif
			}
			//regSetDwordValue(RAMWX, ramw_x); TODO
			//regSetDwordValue(RAMWY, ramw_y); TODO
		}	break;

		case WM_INITDIALOG: 
		{
			if (RWSaveWindowPos)
			{
				POINT pt = { ramw_x, ramw_y };
				pt = CalcSubWindowPos(hDlg, &pt);
				ramw_x = pt.x;
				ramw_y = pt.y;
			}
			else
				CalcSubWindowPos(hDlg, NULL);

			ramwatchmenu=GetMenu(hDlg);
			rwrecentmenu=CreateMenu();
			UpdateRW_RMenu(rwrecentmenu, RAMMENU_FILE_RECENT, RW_MENU_FIRST_RECENT_FILE);
			
			const char* names[3] = {"Address","Value","Notes"};
			int widths[3] = {78,64,160};
			init_list_box(GetDlgItem(hDlg,IDC_WATCHLIST),names,3,widths);

			SeparatorCache::Init(GetDlgItem(hDlg, IDC_WATCHLIST));

			if (!ResultCount)
				reset_address_info();
			else
				signal_new_frame();
			ListView_SetItemCount(GetDlgItem(hDlg,IDC_WATCHLIST),WatchCount);
			if (!noMisalign) SendDlgItemMessage(hDlg, IDC_MISALIGN, BM_SETCHECK, BST_CHECKED, 0);
			//if (littleEndian) SendDlgItemMessage(hDlg, IDC_ENDIAN, BM_SETCHECK, BST_CHECKED, 0);

			RamWatchAccels = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_RWACCELERATOR));

			// due to some bug in windows, the arrow button width from the resource gets ignored, so we have to set it here
			SetWindowPos(GetDlgItem(hDlg,ID_WATCHES_UPDOWN), 0,0,0, 30,60, SWP_NOMOVE);

			Update_RAM_Watch();

			DragAcceptFiles(hDlg, TRUE);

			RefreshWatchListSelectedCountControlStatus(hDlg);

			return false;
		}	break;
		
		case WM_INITMENU:
			CheckMenuItem(ramwatchmenu, RAMMENU_FILE_AUTOLOAD, AutoRWLoad ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem(ramwatchmenu, RAMMENU_FILE_SAVEWINDOW, RWSaveWindowPos ? MF_CHECKED : MF_UNCHECKED);
			break;

		case WM_MENUSELECT:
 		case WM_ENTERSIZEMOVE:
			//Clear_Sound_Buffer();
			break;
		case WM_NOTIFY:
		{
			switch(wParam)
			{
				case ID_WATCHES_UPDOWN:
				{
					switch(((LPNMUPDOWN)lParam)->hdr.code)
					{
						case UDN_DELTAPOS: {
							int delta = ((LPNMUPDOWN)lParam)->iDelta;
							SendMessage(hDlg, WM_COMMAND, delta<0 ? IDC_C_WATCH_UP : IDC_C_WATCH_DOWN,0);
						} break;
					}
				} break;

				case IDC_WATCHLIST:
				{
					LPNMHDR lP = (LPNMHDR) lParam;
					switch (lP->code)
					{
						case LVN_ITEMCHANGED: // selection changed event
						{
							NMLISTVIEW* pNMListView = (NMLISTVIEW*)lP;

							if(pNMListView->uNewState & LVIS_FOCUSED ||
								(pNMListView->uNewState ^ pNMListView->uOldState) & LVIS_SELECTED)
								// disable buttons that we don't have the right number of selected items for
								RefreshWatchListSelectedCountControlStatus(hDlg, pNMListView->iItem);

						}	break;

						case LVN_GETDISPINFO:
						{
							NMLVDISPINFO *Item = (NMLVDISPINFO*)lParam;
							const unsigned int iNum = Item->item.iItem;
							if (rswatches[iNum].Type != 'S')
							{
								Item->item.mask = LVIF_TEXT;
								static char num[11];
								switch (Item->item.iSubItem)
								{
									case 0: {
										int size = WatchSizeConv(rswatches[iNum]);
										int addr = rswatches[iNum].Address;
										sprintf(num, size > 1 ? "%04X-%04X" : "%04X", addr, addr + size - 1);
										Item->item.pszText = num;
										break;
									}
									case 1: {
										int i = rswatches[iNum].CurValue;
										int t = rswatches[iNum].Type;
										int size = rswatches[iNum].Size;
										if (rswatches[iNum].Type == 'b')
										{
											auto bits = bitset<8>(i); // Currently work with 1 byte size only
											int j = 0, k = 7;
											while (k >= 0)
											{		
												if (k == 3) num[j++] = ' ';
												num[j++] = bits[k--] ? '1' : '0';
											}
											Item->item.pszText = num;
											break;
										}
										const char* formatString = ((t == 's') ? "%d" : (t == 'u') ? "%u" : (size == 'd' ? "%08X" : size == 'w' ? "%04X" : "%02X"));
										switch (size)
										{
										case 'b':
										default: sprintf(num, formatString, t == 's' ? (char)(i & 0xff) : (unsigned char)(i & 0xff)); break;
										case 'w': sprintf(num, formatString, t == 's' ? (short)(i & 0xffff) : (unsigned short)(i & 0xffff)); break;
										case 'd': sprintf(num, formatString, t == 's' ? (long)(i & 0xffffffff) : (unsigned long)(i & 0xffffffff)); break;
										}

										Item->item.pszText = num;
									}
									break;
									case 2:
										Item->item.pszText = rswatches[iNum].comment ? rswatches[iNum].comment : (char*)"";
										break;
									default:
										break;
								}
							}
							return true;
						}
						case LVN_ODFINDITEM:
						{	
							// disable search by keyboard typing,
							// because it interferes with some of the accelerators
							// and it isn't very useful here anyway
							SetWindowLongPtr(hDlg, DWLP_MSGRESULT, ListView_GetSelectionMark(GetDlgItem(hDlg,IDC_WATCHLIST)));
							return 1;
						}
						case NM_SETFOCUS:
							listFocus = true;
							InvalidateRect(GetDlgItem(hDlg, IDC_WATCHLIST), 0, 0);
							break;
						case NM_KILLFOCUS:
							listFocus = false;
							InvalidateRect(GetDlgItem(hDlg, IDC_WATCHLIST), 0, 0);
							break;
						case NM_CUSTOMDRAW:
						{
							NMCUSTOMDRAW* nmcd = (NMCUSTOMDRAW*)lParam;
							switch (nmcd->dwDrawStage)
							{
								case CDDS_PREPAINT:
									SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
								break;
								case CDDS_ITEMPREPAINT:
									if (rswatches[nmcd->dwItemSpec].Type == 'S')
										// A separator looks very different from normal watches, it should be drawn in another space while I want to use the highlight bar and the focus frame from the system.
										SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_NOTIFYPOSTPAINT);
									else
									{
										NMLVCUSTOMDRAW* lplvcd = (NMLVCUSTOMDRAW*)lParam;
										switch (rswatches[nmcd->dwItemSpec].Cheats)
										{
											default:
											case 0:
												SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_DODEFAULT);
												return TRUE;
											case 1:
												lplvcd->clrTextBk = RGB(216, 203, 253); break;
											case 2:
												lplvcd->clrTextBk = RGB(195, 186, 253); break;
											case 3:
												lplvcd->clrTextBk = RGB(176, 139, 252); break;
											case 4:
												lplvcd->clrTextBk = RGB(175, 94, 253);
												lplvcd->clrText = RGB(255, 255, 255); break; // use a more visual color in dark background
										}
										SetWindowLongPtr(hDlg, DWLP_MSGRESULT, CDRF_NEWFONT);
									}
								break;
								case CDDS_ITEMPOSTPAINT:
									// Here is the separator actually drawn
									if (rswatches[nmcd->dwItemSpec].Type == 'S')
									{
										char* comment = rswatches[nmcd->dwItemSpec].comment;
										RECT rect;
										rect.left = LVIR_BOUNDS;
										SendDlgItemMessage(hDlg, IDC_WATCHLIST, LVM_GETITEMRECT, nmcd->dwItemSpec, (LPARAM)&rect);

										HDC hdc = nmcd->hdc;

										// This value is required, when you click outside any items in the list there's nothing selected, but the frame is still on the separator, it would draw with the wrong color. I don't know if there's a better way to check this state, since nmcd->uItemState doesn't seem to have a value for it.
										bool state = SendDlgItemMessage(hDlg, IDC_WATCHLIST, LVM_GETITEMSTATE, nmcd->dwItemSpec, LVIS_SELECTED) && listFocus;

										SeparatorCache& sepCache = separatorCache[nmcd->dwItemSpec];

										// draw the separator
										// draw it with a different color when hilighted for eyes easy
										SelectObject(hdc, state ? SeparatorCache::sepPenSel : SeparatorCache::sepPen);
										MoveToEx(hdc, rect.left + sepCache.sepOffX, rect.top + SeparatorCache::sepOffY, NULL);
										LineTo(hdc, rect.right, rect.top + SeparatorCache::sepOffY);

										// draw the comment as the separator title
										if (comment && comment[0])
										{
											rect.top += sepCache.labelOffY;
											rect.left += 6;
											// draw it with a different color when hilighted for eyes easy
											SetTextColor(hdc, state ? RGB(229, 224, 236) : RGB(43, 145, 175));
											SelectObject(hdc, SeparatorCache::sepFon);
											DrawText(hdc, comment, strlen(comment), &rect, DT_LEFT);
										}
									}
									break;

							}
							return TRUE;
						}
					}
				}
			}
		}
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case RAMMENU_FILE_SAVE:
					QuickSaveWatches();
					break;
				case RAMMENU_FILE_SAVEAS:	
					return Save_Watches();
				case RAMMENU_FILE_OPEN:
					return Load_Watches(true);
				case RAMMENU_FILE_APPEND:
					return Load_Watches(false);
				case RAMMENU_FILE_NEW:
					ResetWatches();
					return true;
				case IDC_C_WATCH_REMOVE:
				{
					HWND watchListControl = GetDlgItem(hDlg, IDC_WATCHLIST);
					watchIndex = ListView_GetNextItem(watchListControl, -1, LVNI_ALL | LVNI_SELECTED);
					while (watchIndex >= 0)
					{
						RemoveWatch(watchIndex);
						ListView_DeleteItem(watchListControl, watchIndex);
						watchIndex = ListView_GetNextItem(watchListControl, -1, LVNI_ALL | LVNI_SELECTED);
					}
					RWfileChanged = true;
					SetFocus(GetDlgItem(hDlg,IDC_WATCHLIST));
					return true;
				}
				case IDC_C_WATCH_EDIT:
					watchIndex = ListView_GetSelectionMark(GetDlgItem(hDlg,IDC_WATCHLIST));
					if(watchIndex != -1)
					{
						DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EDITWATCH), hDlg, EditWatchProc, watchIndex);
						SetFocus(GetDlgItem(hDlg,IDC_WATCHLIST));
					}
					return true;
				case IDC_C_WATCH:
				{
					AddressWatcher& target = rswatches[WatchCount];
					target.Address = 0;
					target.WrongEndian = 0;
					target.Size = 'b';
					target.Type = 's';
					DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EDITWATCH), hDlg, EditWatchProc, WatchCount);
					SetFocus(GetDlgItem(hDlg, IDC_WATCHLIST));
					return true;
				}
				case IDC_C_WATCH_DUPLICATE:
				{
					watchIndex = ListView_GetSelectionMark(GetDlgItem(hDlg, IDC_WATCHLIST));
					if (watchIndex != -1)
					{
						char str_tmp[1024];
						AddressWatcher* target = &rswatches[WatchCount];
						AddressWatcher* source = &rswatches[watchIndex];
						memcpy(target, source, sizeof(AddressWatcher));
						target->comment = strcpy(str_tmp, source->comment);
						DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EDITWATCH), hDlg, EditWatchProc, WatchCount);
						SetFocus(GetDlgItem(hDlg, IDC_WATCHLIST));
					}
					return true;
				}
				case IDC_C_WATCH_SEPARATE:
				{
					AddressWatcher* target = &rswatches[WatchCount];
					target->Address = 0;
					target->WrongEndian = false;
					target->Size = 'S';
					target->Type = 'S';

					DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EDITWATCH), hDlg, EditWatchProc, (LPARAM)WatchCount);

					// InsertWatch(separator, "----------------------------");
					SetFocus(GetDlgItem(hDlg, IDC_WATCHLIST));
					return true;
				}
				case IDC_C_WATCH_UP:
				{
					watchIndex = ListView_GetSelectionMark(GetDlgItem(hDlg,IDC_WATCHLIST));
					if (watchIndex == 0 || watchIndex == -1)
						return true;
					void *tmp = malloc(sizeof(AddressWatcher));
					memcpy(tmp, &rswatches[watchIndex], sizeof(AddressWatcher));
					memcpy(&rswatches[watchIndex], &rswatches[watchIndex - 1], sizeof(AddressWatcher));
					memcpy(&rswatches[watchIndex - 1], tmp, sizeof(AddressWatcher));
					free(tmp);

					tmp = malloc(sizeof(SeparatorCache));
					memcpy(tmp, &separatorCache[watchIndex], sizeof(SeparatorCache));
					memcpy(&separatorCache[watchIndex], &separatorCache[watchIndex - 1], sizeof(SeparatorCache));
					memcpy(&separatorCache[watchIndex - 1], tmp, sizeof(SeparatorCache));
					free(tmp);

					ListView_SetItemState(GetDlgItem(hDlg, IDC_WATCHLIST), watchIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);
					ListView_SetSelectionMark(GetDlgItem(hDlg, IDC_WATCHLIST), watchIndex - 1);
					ListView_SetItemState(GetDlgItem(hDlg, IDC_WATCHLIST), watchIndex - 1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
					ListView_SetItemCount(GetDlgItem(hDlg, IDC_WATCHLIST), WatchCount);
					RWfileChanged=true;
					return true;
				}
				case IDC_C_WATCH_DOWN:
				{
					watchIndex = ListView_GetSelectionMark(GetDlgItem(hDlg, IDC_WATCHLIST));
					if (watchIndex >= WatchCount - 1 || watchIndex == -1)
						return true;

					void *tmp = calloc(1, sizeof(AddressWatcher));
					memcpy(tmp, &rswatches[watchIndex], sizeof(AddressWatcher));
					memcpy(&rswatches[watchIndex], &rswatches[watchIndex + 1], sizeof(AddressWatcher));
					memcpy(&rswatches[watchIndex + 1], tmp, sizeof(AddressWatcher));
					free(tmp);

					tmp = calloc(1, sizeof(SeparatorCache));
					memcpy(tmp, &separatorCache[watchIndex], sizeof(SeparatorCache));
					memcpy(&separatorCache[watchIndex], &separatorCache[watchIndex + 1], sizeof(SeparatorCache));
					memcpy(&separatorCache[watchIndex + 1], tmp, sizeof(SeparatorCache));
					free(tmp);

					ListView_SetItemState(GetDlgItem(hDlg, IDC_WATCHLIST), watchIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);
					ListView_SetSelectionMark(GetDlgItem(hDlg, IDC_WATCHLIST),watchIndex + 1);
					ListView_SetItemState(GetDlgItem(hDlg, IDC_WATCHLIST), watchIndex + 1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
					ListView_SetItemCount(GetDlgItem(hDlg, IDC_WATCHLIST), WatchCount);
					RWfileChanged = true;
					return true;
				}
				case ID_WATCHES_UPDOWN:
				{
					SendMessage(hDlg, WM_COMMAND, ((LPNMUPDOWN)lParam)->iDelta < 0 ? IDC_C_WATCH_UP : IDC_C_WATCH_DOWN,0);
					break;
				}
				case RAMMENU_FILE_AUTOLOAD:
				{
					AutoRWLoad ^= 1;
					CheckMenuItem(ramwatchmenu, RAMMENU_FILE_AUTOLOAD, AutoRWLoad ? MF_CHECKED : MF_UNCHECKED);
					//regSetDwordValue(AUTORWLOAD, AutoRWLoad); TODO
					break;
				}
				case RAMMENU_FILE_SAVEWINDOW:
				{
					RWSaveWindowPos ^= 1;
					CheckMenuItem(ramwatchmenu, RAMMENU_FILE_SAVEWINDOW, RWSaveWindowPos ? MF_CHECKED : MF_UNCHECKED);
					//regSetDwordValue(RWSAVEPOS, RWSaveWindowPos); TODO
					break;
				}
				case IDC_C_ADDCHEAT:
				{
					watchIndex = ListView_GetSelectionMark(GetDlgItem(hDlg,IDC_WATCHLIST));
					if(watchIndex >= 0)
						if (rswatches[watchIndex].Type != 'S')
						{
							unsigned int address = rswatches[watchIndex].Address;

							int sizeType = -1;
							if(rswatches[watchIndex].Size == 'b')
								sizeType = 0;
							else if(rswatches[watchIndex].Size == 'w')
								sizeType = 1;
							else if(rswatches[watchIndex].Size == 'd')
								sizeType = 2;

							int numberType = -1;
							if(rswatches[watchIndex].Type == 's')
								numberType = 0;
							else if(rswatches[watchIndex].Type == 'u')
								numberType = 1;
							else if(rswatches[watchIndex].Type == 'h')
								numberType = 2;
							else if (rswatches[watchIndex].Type == 'b')
								numberType = 3;

							// Don't open cheat dialog

							switch (sizeType) {
								case 0:
									FCEUI_AddCheat("", address, rswatches[watchIndex].CurValue, -1, 1);
									break;
								case 1:
									FCEUI_AddCheat("", address, rswatches[watchIndex].CurValue & 0xFF, -1, 1);
									FCEUI_AddCheat("", address + 1, (rswatches[watchIndex].CurValue & 0xFF00) / 0x100, -1, 1);
									break;
								case 2:
									FCEUI_AddCheat("", address, rswatches[watchIndex].CurValue & 0xFF, -1, 1);
									FCEUI_AddCheat("", address + 1, (rswatches[watchIndex].CurValue & 0xFF00) / 0x100, -1, 1);
									FCEUI_AddCheat("", address + 2, (rswatches[watchIndex].CurValue & 0xFF0000) / 0x10000, -1, 1);
									FCEUI_AddCheat("", address + 3, (rswatches[watchIndex].CurValue & 0xFF000000) / 0x1000000, -1, 1);
									break;
							}

							UpdateCheatsAdded();
							UpdateCheatRelatedWindow();

						}
						else
							MessageBox(hDlg, "Sorry, you can't add cheat to a separator.", "Ram Watch", MB_ICONERROR | MB_OK);
				}
				break;
				case IDOK:
				case IDCANCEL:
					RamWatchHWnd = NULL;
					DragAcceptFiles(hDlg, FALSE);
					EndDialog(hDlg, true);
					return true;
				default:
					if (LOWORD(wParam) >= RW_MENU_FIRST_RECENT_FILE && LOWORD(wParam) < RW_MENU_FIRST_RECENT_FILE+MAX_RECENT_WATCHES && LOWORD(wParam) <= RW_MENU_LAST_RECENT_FILE)
					OpenRWRecentFile(LOWORD(wParam) - RW_MENU_FIRST_RECENT_FILE);
			}
			break;

		case WM_KEYDOWN: // handle accelerator keys
		{
			SetFocus(GetDlgItem(hDlg,IDC_WATCHLIST));
			MSG msg;
			msg.hwnd = hDlg;
			msg.message = uMsg;
			msg.wParam = wParam;
			msg.lParam = lParam;
			if(RamWatchAccels && TranslateAccelerator(hDlg, RamWatchAccels, &msg))
				return true;
		}	break;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			break;
		case WM_DESTROY:
			// this is the correct place
			RamWatchHWnd = NULL;
			DragAcceptFiles(hDlg, FALSE);
			WriteRecentRWFiles();	// write recent menu to ini

			// release the hdc related objects
			SeparatorCache::DeInit();
			break;

		case WM_DROPFILES:
		{
			char Str_Tmp[1024];
			HDROP hDrop = (HDROP)wParam;
			DragQueryFile(hDrop, 0, Str_Tmp, 1024);
			DragFinish(hDrop);
			return Load_Watches(true, Str_Tmp);
		}	break;
	}

	return false;
}

// TODO: This function may be too slow when both cheats and watches are near the max limit.
void UpdateWatchCheats() {
	extern int FCEU_CalcCheatAffectedBytes(uint32, uint32);
	for (int i = 0; i < WatchCount; ++i)
		rswatches[i].Cheats = FCEU_CalcCheatAffectedBytes(rswatches[i].Address, WatchSizeConv(rswatches[i]));
}

void SeparatorCache::Init(HWND hBox)
{
	if (!iHeight)
	{
		RECT ir;
		ir.left = LVIR_BOUNDS;

		int count = SendMessage(hBox, LVM_GETITEMCOUNT, 0, 0);
		SendMessage(hBox, LVM_SETITEMCOUNT, 1, 0);
		SendMessage(hBox, LVM_GETITEMRECT, 0, (LPARAM)&ir);
		SendMessage(hBox, LVM_SETITEMCOUNT, count, 0);

		iHeight = ir.bottom - ir.top;
	}

	if (!sepOffY)
		sepOffY = iHeight / 2;

	if (!sepPen)
		sepPen = CreatePen(PS_SOLID, 1, RGB(160, 160, 160));
	if (!sepPenSel)
		sepPenSel = CreatePen(PS_SOLID, 1, RGB(224, 224, 224));

	if (!sepFon)
	{
		LOGFONT logFont;
		GetObject((HANDLE)SendMessage(hBox, WM_GETFONT, NULL, NULL), sizeof(logFont), &logFont);
		sepFon = (HFONT)CreateFontIndirect((logFont.lfWeight = FW_SEMIBOLD, &logFont));
	}
}

void SeparatorCache::DeInit()
{
	DeleteObject(sepPen);
	DeleteObject(sepPenSel);
	DeleteObject(sepFon);

	sepPen = NULL;
	sepPenSel = NULL;
	sepFon = NULL;
}


SeparatorCache::SeparatorCache(HWND hwnd, char* text) {
	if (text && text[0])
	{
		SIZE size;

		HDC hdc = GetDC(hwnd);
		SelectFont(hdc, sepFon);
		GetTextExtentPoint(hdc, text, strlen(text), &size);
		ReleaseDC(hwnd, hdc);

		sepOffX = size.cx + 8;
		labelOffY = (iHeight - size.cy) / 2;
	}
	else {
		// Is there a way to find real ident of the highlight mark in the first cloumn?
		sepOffX = 4;
		labelOffY = 0;
	}
}
