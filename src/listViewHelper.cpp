/* List-view functions: */
#define ListView HWND
/* Unique IDs: */
#define LST_PROCS_ID 1001
#define LST_LIBS_ID 1002


bool InitListViewColumns(ListView, char**, int);
ListView CreateListView(HWND, int, int, int, int, int);
bool InsertListViewItems(ListView, int);

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
