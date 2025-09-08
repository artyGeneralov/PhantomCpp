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
      RECT ClientRect;
      GetClientRect(Window, &ClientRect);
      int x = ClientRect.left;           
      int y = ClientRect.top;            
      int width = ClientRect.right - x;  
      int height = ClientRect.bottom - y;
      ResizeDIBSection(width, height);
      
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
				       WS_CHILD | LVS_REPORT | LVS_EDITLABEL,
				       x, y, width, height,
				       parentHandle, 0, MainProgramInstance, 0);

  return(listViewHandle);
				     
}

bool InitListViewColumns(HWND listViewHandle, char** columns)
{

}
