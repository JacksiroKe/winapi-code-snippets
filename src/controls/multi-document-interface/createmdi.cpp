#include <windows.h>  //include all the basics
#include <tchar.h>    //string and other mapping macros
#include <string>
#include "resources.h"

//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//main window message processing function declarations
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void OnCommand(const HWND,int,int,const HWND);
int OnCreate(const HWND,CREATESTRUCT*);

//mdi child window message processing declarations
LRESULT CALLBACK MdiChildWndProc_01(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK MdiChildWndProc_02(HWND,UINT,WPARAM,LPARAM);

//non-message function declarations
HWND CreateMDIChildWnd(const HWND,const HINSTANCE,DWORD,const RECT&,const int,
                       const ustring&,const ustring&);
HWND CreateMDIClient(const HWND,const HINSTANCE);
inline int ErrMsg(const ustring&);
BOOL CALLBACK MdiCloseChildProc(HWND,LPARAM);  //EnumChildProc
bool RegisterMdiChild(WNDPROC,const ustring&,const HINSTANCE);

//mdi client window identifier
namespace {
  const int IDC_MDI_CLIENT=500;
}
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR pStr,int nCmd)
{
ustring classname=_T("SIMPLEWND");
WNDCLASSEX wcx={0};  //used for storing information about the wnd 'class'

wcx.cbSize         = sizeof(WNDCLASSEX);           
wcx.lpfnWndProc    = WndProc;             //wnd Procedure pointer
wcx.hInstance      = hInst;               //app instance
//use 'LoadImage' to load wnd class icon and cursor as it supersedes the 
//obsolete functions 'LoadIcon' and 'LoadCursor', although these functions will 
//still work. Because the icon and cursor are loaded from system resources ie 
//they are shared, it is not necessary to free the image resources with either 
//'DestroyIcon' or 'DestroyCursor'.
wcx.hIcon         = reinterpret_cast<HICON>(LoadImage(0,IDI_APPLICATION,
                                            IMAGE_ICON,0,0,LR_SHARED));
wcx.hCursor       = reinterpret_cast<HCURSOR>(LoadImage(0,IDC_ARROW,
                                              IMAGE_CURSOR,0,0,LR_SHARED));
wcx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE+1);   
wcx.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);   //wnd menu
wcx.lpszClassName = classname.c_str(); 
//the window 'class' (not c++ class) has to be registered with the system
//before windows of that 'class' can be created
if (!RegisterClassEx(&wcx))
  {
  ErrMsg(_T("Failed to register wnd class"));
  return -1;
  }

int desktopwidth=GetSystemMetrics(SM_CXSCREEN);
int desktopheight=GetSystemMetrics(SM_CYSCREEN);

HWND hwnd=CreateWindowEx(0,                     //extended styles
                         classname.c_str(),     //name: wnd 'class'
                         _T("Multiple Document Interface(MDI)"), //wnd title 
                         WS_OVERLAPPEDWINDOW,   //wnd style
                         desktopwidth/4,        //position:left
                         desktopheight/4,       //position: top
                         desktopwidth/2,        //width
                         desktopheight/2,       //height
                         0,                     //parent wnd handle
                         0,                     //menu handle/wnd id
                         hInst,                 //app instance
                         0);                    //user defined info
if (!hwnd)
  {
  ErrMsg(_T("Failed to create wnd"));
  return -1;
  }

ShowWindow(hwnd,nCmd); 
UpdateWindow(hwnd);

HACCEL hAccel=0;    //no accelerator table in this example. 

//start message loop - windows applications are 'event driven' waiting on user,
//application or system signals to determine what action, if any, to take. Note 
//that an error may cause GetMessage to return a negative value so, ideally,  
//this result should be tested for and appropriate action taken to deal with 
//it(the approach taken here is to simply quit the application). Note the
//extra commands to ensure default mdi keyboard message processing; the 
//TranslateAccelerator command is present only for demonstration purposes as
//there are no application defined accelerator keys in this example.
HWND hMdiClient=GetDlgItem(hwnd,IDC_MDI_CLIENT);
MSG msg;
while (GetMessage(&msg,0,0,0)>0)
  {
  if (!TranslateMDISysAccel(hMdiClient,&msg)&&   //mdi specific keyboard msgs
      !TranslateAccelerator(hwnd,hAccel,&msg))   //all other keyboard msgs
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    }
  }
return static_cast<int>(msg.wParam);
}
//=============================================================================
//main window message processing functions
//=============================================================================
LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
static HWND hMdiClient;

switch (uMsg)
  {
  case WM_COMMAND:
    OnCommand(hwnd,LOWORD(wParam),HIWORD(wParam),
              reinterpret_cast<HWND>(lParam));
    //must call DefFrameProc to ensure that min/max/close default behaviour
    //of mdi child windows can occur.
    return DefFrameProc(hwnd,hMdiClient,WM_COMMAND,wParam,lParam);
  case WM_CREATE:
    hMdiClient=CreateMDIClient(hwnd,GetModuleHandle(0));
    return OnCreate(hwnd,reinterpret_cast<CREATESTRUCT*>(lParam));
  case WM_DESTROY:
    DefFrameProc(hwnd,hMdiClient,WM_COMMAND,wParam,lParam);
    PostQuitMessage(0);    //signal end of application
    return 0;
  default:
    //let system deal with msg
    return DefFrameProc(hwnd,hMdiClient,uMsg,wParam,lParam);
  }
}
//=============================================================================
void OnCommand(const HWND hwnd,int id,int notifycode,const HWND hCntrl)
{
//handles WM_COMMAND message of the main, parent window

const static HWND hMdiClient=GetDlgItem(hwnd,IDC_MDI_CLIENT);

switch (id)
  {
  case IDCANCEL:         //menu item: file --> exit
    DestroyWindow(hwnd);
    break;
  case IDM_WND_TILE:     //menu item: window --> tile
    SendMessage(hMdiClient,WM_MDITILE,0,0);
    break;
  case IDM_WND_CASCADE:  //menu item: window --> cascade
    SendMessage(hMdiClient,WM_MDICASCADE,0,0);
    break;
  case IDM_WND_ARRANGE:  //menu item: window --> arrange
    SendMessage(hMdiClient,WM_MDIICONARRANGE,0,0);
    break;
  case IDM_WND_CLOSEALL:  //menu item: window --> close all
    //EnumChildWindows fn enumerates all child windows of the
    //mdi client window calling the MdiCloseChildProc CALLBACK fn
    //for each child window that exists.
    EnumChildWindows(hMdiClient,MdiCloseChildProc,0) ;
    break;
  }
}
//=============================================================================
int OnCreate(const HWND hwnd,CREATESTRUCT *cs)
{
//handles the WM_CREATE message of the main, parent window; return -1 to fail
//window creation
//
//register and create two mdi child windows. Note that the parent of the mdi 
//child windows is the mdi client window.
HWND hMdiClient=GetDlgItem(hwnd,IDC_MDI_CLIENT);
      
RegisterMdiChild(MdiChildWndProc_01,_T("mdi_child_01"),cs->hInstance);
RECT rc={0,0,400,200};
//create an mdi child window of the class just registered
CreateMDIChildWnd(hMdiClient,cs->hInstance,WS_CLIPCHILDREN,rc,
                  IDM_FIRSTCHILD,_T("MDI Child Window#1"),_T("mdi_child_01"));

RegisterMdiChild(MdiChildWndProc_02,_T("mdi_child_02"),cs->hInstance);
//create an mdi child window of the class just registered
rc.left=40;
rc.top=40;
CreateMDIChildWnd(hMdiClient,cs->hInstance,WS_CLIPCHILDREN,rc,
                  (IDM_FIRSTCHILD+1),_T("MDI Child Window#2"),
                  _T("mdi_child_02"));

return 0;
}
//=============================================================================
//mdi child window message processing functions
//=============================================================================
LRESULT CALLBACK MdiChildWndProc_01(HWND hwnd,UINT uMsg,WPARAM wParam,
                                    LPARAM lParam)
{
static HWND hEdit;

switch (uMsg)
  {
  case WM_CREATE:
    {
    CREATESTRUCT *cs=reinterpret_cast<CREATESTRUCT*>(lParam);
    //make client area an edit control
    hEdit=CreateWindowEx(WS_EX_CLIENTEDGE,
                         _T("edit"),_T("edit control"),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE|WS_VSCROLL,
                         0,0,0,0,hwnd,0,cs->hInstance,0);
    //change the font to default gui font
    SendMessage(hEdit,WM_SETFONT,
                reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)),0);
    return 0;
    }
  case WM_SIZE:
    //change the control extended style so that it doesn't have an extra
    //border when maximized.
    if (wParam==SIZE_MAXIMIZED)
      {
      SetWindowLongPtr(hEdit,GWL_EXSTYLE,0);
      }
    else
      {
      SetWindowLongPtr(hEdit,GWL_EXSTYLE,WS_EX_CLIENTEDGE);
      }
    //ensure edit control always fits exactly within its parent's client 
    //area
    MoveWindow(hEdit,0,0,LOWORD(lParam),HIWORD(lParam),1);
  default:
    return DefMDIChildProc(hwnd,uMsg,wParam,lParam);
  }
}
//=============================================================================
LRESULT CALLBACK MdiChildWndProc_02(HWND hwnd,UINT uMsg,WPARAM wParam,
                                    LPARAM lParam)
{
static HWND hListbox;

switch (uMsg)
  {
  case WM_CREATE:
    {
    CREATESTRUCT *cs=reinterpret_cast<CREATESTRUCT*>(lParam);
    //make client area a listbox control
    hListbox=CreateWindowEx(WS_EX_CLIENTEDGE,
                            _T("listbox"),_T("listbox control"),
                            WS_VISIBLE|WS_CHILD|LBS_NOINTEGRALHEIGHT,
                            0,0,0,0,hwnd,0,cs->hInstance,0);
    //change the font to default gui font
    SendMessage(hListbox,WM_SETFONT,
                (WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
    //add a string to the listbox
    SendMessage(hListbox,LB_ADDSTRING,0,(LPARAM)_T("Listbox"));
    return 0;
    }
  case WM_SIZE:
    //change the control extended style so that it doesn't have an extra
    //border when maximized.
    if (wParam==SIZE_MAXIMIZED)
      {
      SetWindowLongPtr(hListbox,GWL_EXSTYLE,0);
      }
    else
      {
      SetWindowLongPtr(hListbox,GWL_EXSTYLE,WS_EX_CLIENTEDGE);
      }
    //ensure listbox control always fits exactly within its parent's client 
    //area this also ensures any previous change made by SetWindowLongPtr
    //is drawn.
    MoveWindow(hListbox,0,0,LOWORD(lParam),HIWORD(lParam),1);
  default:
    return DefMDIChildProc(hwnd,uMsg,wParam,lParam);
  }
}
//=============================================================================
//non-message processing functions
//=============================================================================
HWND CreateMDIChildWnd(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle,
                       const RECT& rc,const int id,const ustring& caption, 
                       const ustring& classname)
{
dwStyle|=WS_CHILD|WS_VISIBLE;
return CreateWindowEx(WS_EX_MDICHILD,               //extended styles
                      classname.c_str(),            //control 'class' name
                      caption.c_str(),              //control caption
                      dwStyle,                      //control style 
                      rc.left,                      //position: left
                      rc.top,                       //position: top
                      rc.right,                     //width
                      rc.bottom,                    //height
                      hParent,                      //parent window handle
                      //control's ID
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                      hInst,                        //application instance
                      0);                           //user defined info
}
//=============================================================================
HWND CreateMDIClient(const HWND hParent,const HINSTANCE hInst)
{
//In order to get default handling for the menu item called 'window' it is 
//necessary to pass the address of a CLIENTCREATSTRUCT as the 'lpParam' of 
//CreateWindowEx. 
CLIENTCREATESTRUCT ccs; 
ccs.hWindowMenu=GetSubMenu(GetMenu(hParent),1); //menu handle for menu item 
                                                //called 'window'
ccs.idFirstChild=IDM_FIRSTCHILD; //id of the first mdi child window

return CreateWindowEx(0,
                      _T("mdiclient"),
                      0,
                      WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_VSCROLL|
                      WS_HSCROLL,
                      0,0,0,0, 
                      hParent,
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>
                        (IDC_MDI_CLIENT)), 
                      hInst, 
                      reinterpret_cast<void*>(&ccs)); 
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
return MessageBox(0,s.c_str(),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
}
//=============================================================================
BOOL CALLBACK MdiCloseChildProc(HWND hwnd,LPARAM lParam)
{
//EnumChildProc that destroys mdi child windows
SendMessage(GetParent(hwnd),WM_MDIDESTROY,(WPARAM)hwnd,0);
return TRUE;
}
//=============================================================================
bool RegisterMdiChild(WNDPROC wp,const ustring& s,const HINSTANCE hInst)
{
HCURSOR    hCursor;
HICON      hIcon;
WNDCLASSEX wcx={0};

hIcon=(HICON)LoadImage(0,IDI_APPLICATION,IMAGE_ICON,0,0,LR_SHARED);
hCursor=(HCURSOR)LoadImage(0,IDC_ARROW,IMAGE_CURSOR,0,0,LR_SHARED);
wcx.cbSize           = sizeof(WNDCLASSEX);          
wcx.style            = CS_HREDRAW|CS_VREDRAW;       
wcx.lpfnWndProc      = wp;                                                  
wcx.cbWndExtra       = sizeof(HWND);                
wcx.hInstance        = hInst;                      
wcx.hIcon            = hIcon;                       
wcx.hCursor          = hCursor;                                        
wcx.lpszClassName    = s.c_str();                                             

//Register the wnd class with the Windows system
if (!RegisterClassEx(&wcx))
  {
  ErrMsg(_T("Failed to register mdi child wnd class"));
  return false;
  }
return true;
}