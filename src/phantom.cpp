#include <windows.h>
#include <commctrl.h>

/* Useful definitions: */


#define local_persist static
#define global_variable static
#define ListView HWND

/* Menu Items */
#define FILE_MENU_CLOSE 101


/* Unique IDs: */
#define LST_PROCS_ID 1001
#define LST_LIBS_ID 1002

/* Function Prototypes: */
LRESULT CALLBACK MainWindowCallback(HWND, UINT, WPARAM, LPARAM);
void SetupMenu(HWND);
void SetupControls(HWND);
bool InitListViewColumns(ListView, char**, int);
ListView CreateListView(HWND, int, int, int, int, int);
bool InsertListViewItems(ListView, int);

/* Globals: */
global_variable bool IsRunning;
global_variable HMENU Menu;
global_variable HINSTANCE MainProgramInstance;



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
  char* cols1[] = {"name", "id"};
  char* cols2[] = {"dogs", "cats"};
  InitListViewColumns(List1, cols1, sizeof(cols1) / sizeof(cols1[0]));
  InitListViewColumns(List2, cols2, sizeof(cols2) / sizeof(cols2[0]));
  InsertListViewItems(List1, gRowCount);
}


/* List-view functions: */

HWND CreateListView(HWND parentHandle, int uniqueID, int x, int y, int width, int height)
{
  RECT clientRect;
  GetClientRect(parentHandle, &clientRect);

  ListView listViewHandle = CreateWindowEx(0, WC_LISTVIEW, "",
				       WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS,
				       x, y, width, height,
				       parentHandle, (HMENU)uniqueID, MainProgramInstance, 0);

  return(listViewHandle);
				     
}

/* works with the input:
  char* cols[] = {"col1", "col2", "col3" }'
   InitListViewColumns(ListView, cols, sizeof(cols)/sizeof(cols[0]); */
bool InitListViewColumns(ListView listViewHandle, char** columns, int columnCount)
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

bool InsertListViewItems(ListView ListViewHandle, int items_amount)
{
  LVITEM lvItem;
  
  lvItem.mask = LVIF_TEXT | LVIF_PARAM;
  lvItem.pszText = LPSTR_TEXTCALLBACK;
  lvItem.iSubItem = 0;

  for(int i = 0; i < items_amount; i++)
  {
    lvItem.lParam = i;
    lvItem.iItem = i;
    

    if(ListView_InsertItem(ListViewHandle, &lvItem) == -1)
    {
      return false;
    }
  }
  return true;
}
