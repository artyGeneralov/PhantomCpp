#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include "arena.cpp"
#include "globals.hpp"
/* Useful definitions: */
// moved to globals.hpp

/* Menu Items */
#define FILE_MENU_CLOSE 101

/* Unique IDs: */
#define LST_PROCS_ID 1001
#define LST_LIBS_ID 1002
#define BTN_START_ID 2001

/* Function Prototypes: */
LRESULT CALLBACK MainWindowCallback(HWND, UINT, WPARAM, LPARAM);
void SetupMenu(HWND);
void SetupControls(HWND);
bool LV_InitColumns(ListView, char**, int);
ListView LV_Create(HWND, int, int, int, int, int);
void LV_SetItemCount(ListView, int);
HWND CreateButton(HWND, int, int, int, int, int);

/* Globals: */
global_variable bool IsRunning;
global_variable HMENU Menu;
global_variable HINSTANCE MainProgramInstance;
global_variable ListView ListProcesses;
global_variable ListView List2;


/* Process Data structures testing second iteration:  */
// prototypes:
void CreateProcessEntry(char*, int);
void CreateLibraryEntry(int, char*, char*);
int EntryOffsetByPID(int);

typedef struct
{
  DWORD process_name_offset;
  
  DWORD libs_head_offset;
  DWORD libs_tail_offset;
  DWORD libs_count;

  DWORD handles_head;
  DWORD handles_tail;
  DWORD handles_count;
  
} ProcessInfo;

typedef struct
{
  DWORD next_offset;
  DWORD name_offset;
} LibInfo;

typedef struct
{
  DWORD next_offset;
  DWORD name_offset;
} HandleInfo;

typedef struct
{
  DWORD pid;
  Arena processArena;
  DWORD procInfoOffset; // This is supposed to be always 0, but theoretically doesn't have to be
} ProcessEntry;

#define MAX_PROCESSES 1000
global_variable ProcessEntry ProcessEntries[MAX_PROCESSES];
global_variable unsigned int ProcessEntriesCount = 0;



void ProcessExited()
{
  
}

void ProcessCreated()
{
  // TODO: mockup process + lib + handle for it (lib+handle would come from LibCreated() and HandleCreated())
  char processName[] = "myProc";
  int pid = 123;

  char libName[] = "myLib.dll";
  char handleName[] = "myHandle";


  
  CreateProcessEntry(processName, pid);
  
  // Libraries Mockup:
  CreateLibraryEntry(pid, libName, 0); //TODO: delete this
		     
  LV_SetItemCount(ListProcesses, ProcessEntriesCount);
}

void CreateLibraryEntry(int pid, char* libName, char* libPath)
{
  ProcessEntry entry =  ProcessEntries[EntryOffsetByPID(pid)];
  ProcessInfo* procInfoPtr = (ProcessInfo*)ArenaOffsetToPtr(&entry.processArena, entry.procInfoOffset);
  LibInfo* libInfoPtr = (LibInfo*)ARENA_PUSH_STRUCT(&entry.processArena, LibInfo);
  char* libNamePtr = (char*) ArenaPushString(&entry.processArena, libName);
  libInfoPtr->name_offset = ArenaPtrToOffset(&entry.processArena, libNamePtr);
  if(procInfoPtr->libs_count == 0)
  {
    procInfoPtr->libs_head_offset = ArenaPtrToOffset(&entry.processArena, libInfoPtr);
  }
  
  procInfoPtr->libs_tail_offset = ArenaPtrToOffset(&entry.processArena, libInfoPtr); 
  procInfoPtr->libs_count++;
 
}

void CreateProcessEntry(char* processName, int pid)
{
  ProcessEntry entry = {0};
  entry.processArena = ArenaCreate(ARENA_MAX_CAPACITY);
  entry.pid = pid;
  
  ProcessInfo myProcInfo = {0};
  
  ProcessInfo* procInfoPtr =(ProcessInfo*)ARENA_PUSH_STRUCT(&(entry.processArena), ProcessInfo);
  char* procNamePtr = (char*) ArenaPushString(&(entry.processArena), processName);
  procInfoPtr->process_name_offset = ArenaPtrToOffset(&(entry.processArena), procNamePtr);

  ProcessEntries[ProcessEntriesCount++] = entry;
}


//TODO: Not sure if to return the entry ptr or offset in the array
//      to be fair, this array should become an arena later, so offset in an arena for a ProcessEntry could be nice
int EntryOffsetByPID(int pid)
{
  for(int i = 0; i < ProcessEntriesCount; i++)
  {
    if(ProcessEntries[i].pid == pid)
    {
      return(i);
    }
  }
  return -1;
}


/* Program start: */

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
  MainProgramInstance = Instance;
  
  INITCOMMONCONTROLSEX initCommonControls;
  initCommonControls.dwSize = sizeof(initCommonControls);
  initCommonControls.dwICC = ICC_LISTVIEW_CLASSES|ICC_STANDARD_CLASSES;
  InitCommonControlsEx(&initCommonControls);
  
  WNDCLASSEX WindowClass = {};
  WindowClass.cbSize = sizeof(WNDCLASSEX);
  WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
  WindowClass.lpfnWndProc = MainWindowCallback;
  WindowClass.hInstance = Instance;
  //  WindowClass.hIcon = ;
  WindowClass.lpszClassName = "PhantomWindowClass";


    
  if(RegisterClassEx(&WindowClass))
  {
    
    HWND WindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, "MainWindow",
				       WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT,
				       CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0,
				       Menu, Instance, 0);
    if(WindowHandle)
    {
      MSG Message;
      BOOL msgRet;
      
      IsRunning = true;      
      while(IsRunning)
      {
	msgRet = GetMessage(&Message, 0, 0, 0);
	if(msgRet > 0)
	{
	  TranslateMessage(&Message);
	  DispatchMessage(&Message);
	}
	else
	{
	  // some error in message reception
	}
      }
    }
    else
    {
      // fail?
    }
    
  }
  else
  {
    // fail?
  }
    

  return(0);
}


LRESULT CALLBACK MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
  LRESULT Result = 0;
  switch(Message)
  {
    case WM_CREATE:
    {
      SetupMenu(Window);
      SetupControls(Window);
    }break;
    case WM_SIZE:
    {

      OutputDebugStringA("Received WM_SIZE Message\n");
    }break;
    
    case WM_DESTROY:
    {
      //TODO: This might be an error, maybe recreate window?
      OutputDebugStringA("Received WM_DESTROY Message\n");
    }break;
    
    case WM_CLOSE:
    {
      IsRunning = false;
      OutputDebugStringA("Received WM_CLOSE Message\n");
    }break;

    case WM_ACTIVATEAPP:
    {
      OutputDebugStringA("Received WM_ACTIVATEAPP Message\n");
    }break;

    case WM_MOVE:
    {
      OutputDebugStringA("Received WM_MOVE Message\n");
    }break;

    case WM_COMMAND:
    {
      switch(WParam)
      {
	case FILE_MENU_CLOSE:
	{
	  IsRunning = false;
	}break;
	
	case BTN_START_ID:
	{
	  // TODO: un-implemented button functionality
	  ProcessCreated();
	}break;
      }
    }break;

    case WM_PAINT:
    {
      PAINTSTRUCT Paint;
      HDC DeviceContextHandle = BeginPaint(Window, &Paint); //The paint struct is filled for us (out from windows)

      
      EndPaint(Window, &Paint);
      
      OutputDebugStringA("Received WM_PAINT Message\n");
    }break;

    case WM_NOTIFY:
    {
      LPNMHDR hdr = (LPNMHDR)LParam;
      if(hdr->idFrom == LST_PROCS_ID && hdr->code == LVN_GETDISPINFO)
      {
	NMLVDISPINFO* di = (NMLVDISPINFO*)LParam;
	
	int row = di->item.iItem;
	int col = di->item.iSubItem;
	ProcessEntry entry = ProcessEntries[row];
	if(row < 0) break; // TODO: see if this happens?
	local_persist char buf[512]; // TODO: buffer can overflow, solve this.
	ProcessInfo* p = (ProcessInfo*)ArenaOffsetToPtr(&entry.processArena, entry.procInfoOffset);
	if(col == 0) // pid
	{
	  wsprintfA(buf, "%u", entry.pid);
	} else if( col == 1 ) // name
	{
	  lstrcpynA(buf, (char*)ArenaOffsetToPtr(&entry.processArena, p->process_name_offset), sizeof(buf));
	}
	else
	{
	  buf[0] = '\0';
	}
	di->item.pszText = buf;
	  
	/*
	if (row < 0 || (size_t)row >= Model.Count) break;
        ProcessInfo* p = Model.By_index[row];

        local_persist char buf[512];

        if (col == 0) {
            wsprintfA(buf, "%u", p->PID);
        } else if (col == 1) {
            lstrcpynA(buf, p->ImagePath ? p->ImagePath : "", sizeof(buf));
        } else if (col == 2) {
            lstrcpynA(buf, p->ProcessName ? p->ProcessName : "", sizeof(buf));
        } else {
            buf[0] = '\0';
        }

        di->item.pszText = buf;*/
      }
      else if(hdr->idFrom == LST_LIBS_ID)
      {
      }
    }break;

    default:
    {
      Result = DefWindowProc(Window, Message, WParam, LParam);
      OutputDebugStringA("Received other Message\n");
    }
  }
  return(Result);
    
}

void SetupMenu(HWND WindowHandle)
{
  Menu = CreateMenu();

  // File Menu
  HMENU FileMenu = CreateMenu();
  AppendMenu(FileMenu, MF_STRING,(UINT_PTR) FILE_MENU_CLOSE, "Close");


  // Help Menu
  HMENU HelpMenu = CreateMenu();
  AppendMenu(HelpMenu, MF_STRING, (UINT_PTR) 1, "About");
  
  // Main Menu
  AppendMenu(Menu, MF_POPUP,(UINT_PTR)FileMenu, "File");
  AppendMenu(Menu, MF_POPUP, (UINT_PTR)HelpMenu, "Help");
  SetMenu(WindowHandle, Menu);
}


void SetupControls(HWND WindowHandle)
{
  // TODO: this is all temporary
  ListProcesses = LV_Create(WindowHandle, LST_PROCS_ID, 50, 50, 400, 400);
  List2 = LV_Create(WindowHandle, LST_LIBS_ID, 50, 500, 400, 400);
  ListView_SetExtendedListViewStyleEx(ListProcesses, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
  char* cols1[] = {"PID", "Name"};
  char* cols2[] = {"dogs", "cats"};
  LV_InitColumns(ListProcesses, cols1, sizeof(cols1) / sizeof(cols1[0]));
  LV_InitColumns(List2, cols2, sizeof(cols2) / sizeof(cols2[0]));
  Button btnStart = CreateButton(WindowHandle, BTN_START_ID, 500, 50, 100, 40);
 
}

HWND CreateButton(HWND parentHandle, int uniqueID, int x, int y, int width, int height)
{
  Button btnHandle = CreateWindowEx(0, "BUTTON", "Start",
				    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				    x, y, width, height,
				    parentHandle, (HMENU)(UINT_PTR)uniqueID, MainProgramInstance, 0);
  return(btnHandle);
}

/* List-view functions: */

HWND LV_Create(HWND parentHandle, int uniqueID, int x, int y, int width, int height)
{
  /*  RECT clientRect;
  GetClientRect(parentHandle, &clientRect);
  */
  ListView listViewHandle = CreateWindowEx(0, WC_LISTVIEW, "",
					   WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_OWNERDATA,
					   x, y, width, height,
					   parentHandle, (HMENU)(UINT_PTR)uniqueID, MainProgramInstance, 0);

  return(listViewHandle);
				     
}

/* works with the input:
   char* cols[] = {"col1", "col2", "col3" }'
   LV_InitColumns(ListView, cols, sizeof(cols)/sizeof(cols[0]); */
bool LV_InitColumns(ListView listViewHandle, char** columns, int columnCount)
{
  LVCOLUMN lvc;
  lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

  for(int i = 0; i < columnCount; i++)
  {
    lvc.iSubItem = i;
    lvc.pszText = columns[i];
    lvc.cx = 100; // TODO: move width somewhere else

    lvc.fmt = (i < 2) ? LVCFMT_LEFT : LVCFMT_RIGHT;
    if(ListView_InsertColumn(listViewHandle , i, &lvc) == -1)
    {
      return false;
    }
  }
  return true;
}

void LV_SetItemCount(ListView ListViewHandle, int items_amount)
{
  ListView_SetItemCountEx(ListViewHandle, items_amount, 0);
}


