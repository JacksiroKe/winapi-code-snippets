#include <windows.h>  //include all the basics
#include <tchar.h>    //string and other mapping macros
#include <string>

//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//message processing function declarations
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
int OnCreate(const HWND,CREATESTRUCT*);

//non-message function declarations
inline UINT AddString(const HWND,const ustring&);
HWND CreateCombobox(const HWND,const HINSTANCE,DWORD,const RECT&,const int,
                    const ustring&);                    
inline int ErrMsg(const ustring&);
                    
//setup some combobox id's
enum {
  IDCC_SIMPLE=200,
  IDCC_DROPDOWN,
  IDCC_DROPDOWNLIST
};
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR pStr,int nCmd)
{
ustring classname=_T("SIMPLEWND");
WNDCLASSEX wcx={0};  //used for storing information about the wnd 'class'

wcx.cbSize         = sizeof(WNDCLASSEX);           
wcx.lpfnWndProc    = WndProc;             //wnd procedure pointer
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

HWND hwnd=CreateWindowEx(0,                       //extended styles
                         classname.c_str(),       //name: wnd 'class'
                         _T("Combobox Controls"), //wnd title
                         WS_OVERLAPPEDWINDOW,     //wnd style
                         desktopwidth/4,          //position:left
                         desktopheight/4,         //position: top
                         desktopwidth/2,          //width
                         desktopheight/2,         //height
                         0,                       //parent wnd handle
                         0,                       //menu handle/wnd id
                         hInst,                   //app instance
                         0);                      //user defined info
if (!hwnd)
  {
  ErrMsg(_T("Failed to create wnd"));
  return -1;
  }

ShowWindow(hwnd,nCmd); 
UpdateWindow(hwnd);
//start message loop - windows applications are 'event driven' waiting on user,
//application or system signals to determine what action, if any, to take. Note 
//that an error may cause GetMessage to return a negative value so, ideally,  
//this result should be tested for and appropriate action taken to deal with 
//it(the approach taken here is to simply quit the application).
MSG msg;
while (GetMessage(&msg,0,0,0)>0)
  {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  }
return static_cast<int>(msg.wParam);
}
//=============================================================================
LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
switch (uMsg)
  {
  case WM_CREATE:
    return OnCreate(hwnd,reinterpret_cast<CREATESTRUCT*>(lParam));
  case WM_DESTROY:
    PostQuitMessage(0);    //signal end of application
    return 0;
  default:
    //let system deal with msg
    return DefWindowProc(hwnd,uMsg,wParam,lParam);  
  }
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
return MessageBox(0,s.c_str(),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
}
//=============================================================================
int OnCreate(const HWND hwnd,CREATESTRUCT *cs)
{
//handles the WM_CREATE message of the main, parent window; return -1 to fail
//window creation
RECT rc={10,10,200,90};
//the various combobox types are created by simply varying the style bits
HWND hCombo=CreateCombobox(hwnd,cs->hInstance,CBS_SIMPLE,rc,IDCC_SIMPLE,
                           _T(""));
AddString(hCombo,_T("Simple Combobox"));

rc.top+=90;
hCombo=CreateCombobox(hwnd,cs->hInstance,CBS_DROPDOWN,rc,IDCC_DROPDOWN,_T(""));
AddString(hCombo,_T("Drop Down Combobox"));

rc.top+=40;
hCombo=CreateCombobox(hwnd,cs->hInstance,CBS_DROPDOWNLIST,rc,IDCC_DROPDOWNLIST,
                      _T(""));
AddString(hCombo,_T("Drop Down List Combobox"));

return 0;
}
//=============================================================================
HWND CreateCombobox(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle,
                    const RECT& rc,const int id,const ustring& caption)
{
dwStyle|=WS_CHILD|WS_VISIBLE;
return CreateWindowEx(0,                            //extended styles
                      _T("combobox"),               //control 'class' name
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
inline UINT AddString(const HWND hCombo,const ustring& s)
{
return static_cast<UINT>(SendMessage(hCombo,CB_ADDSTRING,0,
                         reinterpret_cast<LPARAM>(s.c_str())));
}
