#include <windows.h>
#include <commctrl.h>

/* Useful definitions: */


#define local_persist static
#define global_variable static

/* Menu Items */
#define FILE_MENU_CLOSE 101

/* Function Prototypes: */
LRESULT CALLBACK MainWindowCallback(HWND, UINT, WPARAM, LPARAM);
void SetupMenu(HWND);
void SetupControls(HWND);
bool InitListViewColumns(HWND, char**, int);
HWND CreateListView(HWND, int, int, int, int);


/* Globals: */
global_variable bool IsRunning;
global_variable HMENU Menu;
global_variable HINSTANCE MainProgramInstance;
int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
  MainProgramInstance = Instance;
  
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
  AppendMenu(FileMenu, MF_STRING, 0, "Close");


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
  HWND List1 = CreateListView(WindowHandle, 50, 50, 100, 100);
  HWND List2 = CreateListView(WindowHandle, 200, 200, 100, 100);
  char* cols1[] = {"name", "id"};
  char* cols2[] = {"dogs", "cats"};
  InitListViewColumns(List1, cols1, sizeof(cols1) / sizeof(cols1[0]));
  InitListViewColumns(List2, cols2, sizeof(cols2) / sizeof(cols2[0]));
}


/* List-view functions: */

HWND CreateListView(HWND parentHandle, int x, int y, int width, int height)
{
  INITCOMMONCONTROLSEX initCommonControls;
  initCommonControls.dwSize = sizeof(initCommonControls);
  initCommonControls.dwICC = ICC_LISTVIEW_CLASSES|ICC_STANDARD_CLASSES;
  InitCommonControlsEx(&initCommonControls);


  RECT clientRect;
  GetClientRect(parentHandle, &clientRect);

  HWND listViewHandle = CreateWindowEx(0, WC_LISTVIEW, "",
				       WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS,
				       x, y, width, height,
				       parentHandle, 0, MainProgramInstance, 0);

  return(listViewHandle);
				     
}

// char* cols[] = {"col1", "col2", "col3" }'
// InitListViewColumns(ListView, cols, sizeof(cols)/sizeof(cols[0]);
bool InitListViewColumns(HWND listViewHandle, char** columns, int columnCount)
{
  LVCOLUMN lvc;
  lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

  for(int i = 0; i < columnCount; i++) // TODO: columns undefined
  {
    lvc.iSubItem = i;
    lvc.pszText = columns[i]; // TODO: currentText undefined
    lvc.cx = 100; // TODO: move width somewhere else

    lvc.fmt = (i < 2) ? LVCFMT_LEFT : LVCFMT_RIGHT;
    if(ListView_InsertColumn(listViewHandle , i, &lvc) == -1)
    {
      return false;
    }
  }
  return true;
}
