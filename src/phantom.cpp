#include <windows.h>
#include <commctrl.h>
#include "arena.cpp"


/* Useful definitions: */


#define local_persist static
#define global_variable static


/* Menu Items */
#define FILE_MENU_CLOSE 101



/* Function Prototypes: */
LRESULT CALLBACK MainWindowCallback(HWND, UINT, WPARAM, LPARAM);
void SetupMenu(HWND);
void SetupControls(HWND);

/* Globals: */
global_variable bool IsRunning;
global_variable HMENU Menu;
global_variable HINSTANCE MainProgramInstance;

#include "listViewHelper.cpp"

/* TODO: List Testing, temporary! */
#define MAX_ROWS 1024
#define NAME_MAX 64

typedef struct ROW {
  char name[NAME_MAX];
  int id;
} ROW;

ROW gRows[MAX_ROWS] = {
  {"Mike",   10},
  {"Jade",   24},
  {"Daniel", 16}
};
int gRowCount = 3;

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

	if(col == 0)
	{
	  di->item.pszText = gRows[row].name;
	}
	else if(col == 1)
	{
	  wsprintfA(di->item.pszText, "%d", gRows[row].id);
	  
	}
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
  AppendMenu(FileMenu, MF_STRING, FILE_MENU_CLOSE, "Close");


  // Help Menu
  HMENU HelpMenu = CreateMenu();
  AppendMenu(HelpMenu, MF_STRING, 1, "About");
  
  // Main Menu
  AppendMenu(Menu, MF_POPUP,(UINT_PTR)FileMenu, "File");
  AppendMenu(Menu, MF_POPUP, (UINT_PTR)HelpMenu, "Help");
  SetMenu(WindowHandle, Menu);
}

void SetupControls(HWND WindowHandle)
{
  // TODO: this is all temporary
  ListView List1 = CreateListView(WindowHandle, LST_PROCS_ID, 50, 50, 400, 400);
  ListView List2 = CreateListView(WindowHandle, LST_LIBS_ID, 50, 500, 400, 400);
  ListView_SetExtendedListViewStyleEx(List1, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
  char* cols1[] = {"name", "id"};
  char* cols2[] = {"dogs", "cats"};
  InitListViewColumns(List1, cols1, sizeof(cols1) / sizeof(cols1[0]));
  InitListViewColumns(List2, cols2, sizeof(cols2) / sizeof(cols2[0]));
  InsertListViewItems(List1, gRowCount);
}


/**/
